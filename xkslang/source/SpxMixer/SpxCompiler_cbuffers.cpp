//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "SPIRV/doc.h"

#include "SpxCompiler.h"

using namespace std;
using namespace xkslang;

//Remove unused cbuffers, merge used cbuffers havind the same name, take resources type out of the cbuffer
bool SpxCompiler::ProcessCBuffers(vector<XkslMixerOutputStage>& outputStages)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_StreamReschuffled && status != SpxRemapperStatusEnum::MixinBeingCompiled_StreamsAndCBuffersAnalysed) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_CBuffersValidated;
    bool success = true;

    unsigned int positionFirstOpFunctionInstruction = 0;

    //=========================================================================================================================
    //=========================================================================================================================
    //get ALL SHADERs' cbuffers
    vector<CBufferTypeData*> listAllShaderCBuffers;
    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) {
        ShaderClassData* shader = *itsh;
        for (auto it = shader->shaderTypesList.begin(); it != shader->shaderTypesList.end(); it++){
            ShaderTypeData* shaderType = *it;
            if (shaderType->isCBufferType())
            {
                CBufferTypeData* cbufferData = shaderType->type->cbufferData;
                if (cbufferData == nullptr) return error("a cbuffer type is missing cbuffer data (block decorate but no CBufferProperties?): " + shaderType->type->GetName());
                
#ifdef XKSLANG_DEBUG_MODE
                if (cbufferData->shaderOwner == nullptr) return error("a cbuffer data is missing reference to its shader owner");
                if (cbufferData->cbufferMembersData != nullptr) return error("a cbuffer members data has already been initialized");
#endif
                cbufferData->correspondingShaderType = shaderType;
                cbufferData->isUsed = false;
                cbufferData->cbufferMembersData = nullptr;
                cbufferData->posOpNameType = -1;
                cbufferData->posOpNameVariable = -1;

                listAllShaderCBuffers.push_back(cbufferData);
            }
        }
    }
    if (listAllShaderCBuffers.size() == 0) return true;  //no cbuffer at all, we can return immediatly

    //flag the USED cbuffers and allocate object necessary to store their data
    vector<CBufferTypeData*> mapUsedCbuffers;
    mapUsedCbuffers.resize(bound(), nullptr);
    bool anyCBufferUsed = false;
    {
        for (unsigned int iStage = 0; iStage < outputStages.size(); iStage++)
        {
            XkslMixerOutputStage* outputStage = &(outputStages[iStage]);
            for (auto itcb = outputStage->listCBuffersAccessed.begin(); itcb != outputStage->listCBuffersAccessed.end(); itcb++)
            {
                anyCBufferUsed = true;
                ShaderTypeData* shaderType = *itcb;
                CBufferTypeData* cbufferData = shaderType->type->cbufferData;

#ifdef XKSLANG_DEBUG_MODE
                if (shaderType->type->GetId() >= mapUsedCbuffers.size()) { error("cbuffer type id is out of bound. Id: " + to_string(shaderType->type->GetId())); break; }
                if (cbufferData->cbufferCountMembers <= 0) { error("invalid count members for cbuffer: " + shaderType->type->GetName()); break; }
#endif

                cbufferData->isUsed = true;
                if (mapUsedCbuffers[shaderType->type->GetId()] == nullptr)
                {
                    cbufferData->cbufferMembersData = new TypeStructMemberArray();
                    cbufferData->cbufferMembersData->members.resize(cbufferData->cbufferCountMembers);

                    //set both type and variable IDs
                    mapUsedCbuffers[shaderType->type->GetId()] = cbufferData;
                    mapUsedCbuffers[shaderType->variable->GetId()] = cbufferData;
                }
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //Retrieve information from the bytecode for all USED cbuffers, and their members
    unsigned int posLatestMemberNameOrDecorate = header_size;
    if (success && anyCBufferUsed)
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) { error("Corrupted bytecode: wordCount is equals to 0"); break; }
#endif

            switch (opCode)
            {
                case spv::OpName:
                {
                    spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        //ShaderTypeData* cbuffer = fdsfsdf;
                        CBufferTypeData* cbufferData = mapUsedCbuffers[id];

                        if (start > posLatestMemberNameOrDecorate) posLatestMemberNameOrDecorate = start;
                        
                        if (cbufferData->correspondingShaderType->type->GetId() == id) cbufferData->posOpNameType = start;
                        else if (cbufferData->correspondingShaderType->variable->GetId() == id) cbufferData->posOpNameVariable = start;
                    }
                    break;
                }

                case spv::OpMemberAttribute:
                {
                    const spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = mapUsedCbuffers[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string attribute = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
                        if (cbufferData->cbufferMembersData->members[index].attribute.size() > 0) { error("A cbuffer member has more than 1 attribute (not implementet yet)"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].attribute = attribute;
                    }
                    break;
                }

                case spv::OpMemberLinkName:
                case spv::OpMemberLogicalGroup:
                {
                    spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = mapUsedCbuffers[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string name = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        if (opCode == spv::OpMemberLinkName) cbufferData->cbufferMembersData->members[index].linkName = name;
                        else cbufferData->cbufferMembersData->members[index].logicalGroup = name;
                    }
                    break;
                }

                case spv::OpMemberSamplerStateDef:
                {
                    spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = mapUsedCbuffers[id];

                        unsigned int index = asLiteralValue(start + 2);
#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].samplerStateDestBytecodePos = start;

                    }
                    break;
                }

                /*case spv::OpMemberProperties:
                {
                    const spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        const ShaderTypeData* cbuffer = mapUsedCbuffers[id];
                        CBufferTypeData* cbufferData = cbuffer->type->cbufferData;

                        unsigned int index = asLiteralValue(start + 2);
                        string memberName = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (cbuffer->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        //we're only interested by the member's stage property
                        int countProperties = wordCount - 3;
                        for (int a = 0; a < countProperties; ++a)
                        {
                            int prop = asLiteralValue(start + 3 + a);
                            switch (prop)
                            {
                                case spv::XkslPropertyEnum::PropertyStage:
                                    cbufferData->cbufferMembersData->members[index].isStage = true;
                                    break;
                            }
                        }
                    }
                    break;
                }*/

                case spv::OpMemberName:
                {
                    spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = mapUsedCbuffers[id];

                        if (start > posLatestMemberNameOrDecorate) posLatestMemberNameOrDecorate = start;

                        unsigned int index = asLiteralValue(start + 2);
                        string memberName = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].declarationName = memberName;
                    }
                    break;
                }

                case spv::OpMemberDecorate:
                {
                    spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = mapUsedCbuffers[id];
                        if (start > posLatestMemberNameOrDecorate) posLatestMemberNameOrDecorate = start;

                        const unsigned int index = asLiteralValue(start + 2);
                        const spv::Decoration dec = (spv::Decoration)asLiteralValue(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif

                        switch (dec)
                        {
                            case spv::DecorationOffset:
                            {
                                //skip it: the offset will be recomputed for all cbuffer members
                                /*
#ifdef XKSLANG_DEBUG_MODE
                                if (wordCount <= 4) { error("Invalid wordCount"); break; }
#endif
                                const unsigned int offsetValue = asLiteralValue(start + 4);
                                cbufferData->cbufferMembersData->members[index].memberOffset = offsetValue;
                                */
                                break;
                            }
                            case spv::DecorationRowMajor:
                            {
                                cbufferData->cbufferMembersData->members[index].matrixLayoutDecoration = (int)(spv::DecorationRowMajor);
                                break;
                            }
                            case spv::DecorationColMajor:
                            {
                                cbufferData->cbufferMembersData->members[index].matrixLayoutDecoration = (int)(spv::DecorationColMajor);
                                break;
                            }
                            case spv::DecorationMatrixStride:
                            {
                                //skip it: the matrixStride will be recomputed for all cbuffer members
                                /*
#ifdef XKSLANG_DEBUG_MODE
                                if (wordCount <= 4) { error("Invalid wordCount"); break; }
#endif
                                const unsigned int matrixStride = asLiteralValue(start + 4);
                                cbufferData->cbufferMembersData->members[index].matrixStride = matrixStride;
                                */
                                break;
                            }
                            default:
                            {
                                error(string("Unprocessed decoration: ") + DecorationString(dec));
                                break;

                                /*
                                //add the decorations in the list of member decorations to pass
#ifdef XKSLANG_DEBUG_MODE
                                if (wordCount <= 3) { error("Invalid wordCount"); break; }
#endif
                                unsigned int countRemainingBytes = wordCount - 3;
                                std::vector<unsigned int>& listMemberDecoration = cbufferData->cbufferMembersData->members[index].listMemberDecoration;
                                listMemberDecoration.push_back(countRemainingBytes);
                                for (unsigned int i = 0; i < countRemainingBytes; i++)
                                    listMemberDecoration.push_back(asLiteralValue(start + 3 + i));

                                break;
                                */
                            }
                        }
                    }
                    break;
                }

                //done below instead
                /*
                case spv::OpTypeStruct:
                {
                    spv::Id id = asId(start + 1);
                    if (mapUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = mapUsedCbuffers[id];

                        int countMembers = wordCount - 2;
#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (countMembers != cbufferData->cbufferMembersData->members.size()) { error("inconsistent number of members between OpTypeStruct instruction and cbuffer properties"); break; }
#endif
                        for (int m = 0; m < countMembers; ++m)
                        {
                            TypeStructMember& member = cbufferData->cbufferMembersData->members[m];
                            spv::Id memberTypeId = asLiteralValue(start + 2 + m);
                            member.memberTypeId = memberTypeId;

                            //get the member type object
                            TypeInstruction* memberType = GetTypeById(memberTypeId);
                            if (memberType == nullptr) { error("failed to find the member type for memberTypeId: " + to_string(memberTypeId)); break; }
                            member.memberType = memberType;

                            //check if the member type is a resource
                            member.isResourceType = IsResourceType(memberType->opCode);
                        }
                    }
                    break;
                }
                */

                case spv::OpFunction:
                {
                    //all information retrieved at this point: can safely stop here
                    if (positionFirstOpFunctionInstruction == 0) positionFirstOpFunctionInstruction = start;
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }

        if (positionFirstOpFunctionInstruction == 0) positionFirstOpFunctionInstruction = header_size;
        if (errorMessages.size() > 0) success = false;
    }

    //set the members' size and alignment for all used cbuffers
    if (success)
    {
        vector<unsigned int> startPositionOfAllMemberDecorateInstructions;
        if (!GetStartPositionOfAllMemberDecorateInstructions(startPositionOfAllMemberDecorateInstructions))
        {
            error("failed to call GetStartPositionOfAllMemberDecorateInstructions");
            success = false;
        }

        if (success)
        {
            for (unsigned int i = 0; i < listAllShaderCBuffers.size(); i++)
            {
                CBufferTypeData* cbufferData = listAllShaderCBuffers[i];
                if (cbufferData->isUsed)
                {
                    TypeInstruction* cbufferType = GetTypeById(cbufferData->cbufferTypeId);
                    if (cbufferType == nullptr) { error("failed to find the cbuffer type for cbufferTypeId: " + to_string(cbufferData->cbufferTypeId)); break; }

                    spv::Op opCode = asOpCode(cbufferType->GetBytecodeStartPosition());
                    int wordCount = asWordCount(cbufferType->GetBytecodeStartPosition());
                    int countMembers = wordCount - 2;
                
#ifdef XKSLANG_DEBUG_MODE
                    if (opCode != spv::OpTypeStruct) { error(string("Invalid OpCode type for the cbuffer instruction (expected OpTypeStruct): ") + OpcodeString(opCode)); break; }
                    if (countMembers != cbufferData->cbufferCountMembers) { error("Invalid cbuffer count members property"); break; }
#endif
                    if (countMembers != cbufferData->cbufferMembersData->countMembers()) { error("Inconsistent number of members"); break; }

                    //find the size and alignment for the cbuffer members
                    unsigned int posElemStart = cbufferType->GetBytecodeStartPosition() + 2;
                    for (int m = 0; m < countMembers; ++m)
                    {
                        TypeStructMember& member = cbufferData->cbufferMembersData->members[m];

                        //get the member type object
                        spv::Id cbufferMemberTypeId = asId(posElemStart + m);
                        TypeInstruction* cbufferMemberType = GetTypeById(cbufferMemberTypeId);
                        if (cbufferMemberType == nullptr) { error("failed to find the cbuffer element type for id: " + to_string(cbufferMemberTypeId)); break; }

                        member.memberTypeId = cbufferMemberTypeId;
                        member.memberType = cbufferMemberType;

                        //check if the member type is a resource
                        member.isResourceType = IsResourceType(cbufferMemberType->opCode);

                        bool isMatrixRowMajor = true;
                        if (IsMatrixType(cbufferMemberType) || IsMatrixArrayType(cbufferMemberType))
                        {
                            if (member.matrixLayoutDecoration == (int)(spv::DecorationRowMajor)) isMatrixRowMajor = true;
                            else if (member.matrixLayoutDecoration == (int)(spv::DecorationColMajor)) isMatrixRowMajor = false;
                            else { error("undefined matrix member layout"); break; }
                        }
                    
                        {
                            TypeReflectionDescription* typeReflectionData = new TypeReflectionDescription();
                            if (!GetTypeReflectionDescription(cbufferMemberType, isMatrixRowMajor, &(member.attribute), *typeReflectionData, &startPositionOfAllMemberDecorateInstructions))
                            {
                                error("Failed to get the size and alignment for the cbuffer member: " + to_string(cbufferMemberTypeId));
                                break;
                            }

                            cbufferData->cbufferMembersData->members[m].memberSize = typeReflectionData->Size;
                            cbufferData->cbufferMembersData->members[m].memberAlignment = typeReflectionData->Alignment;
                            cbufferData->cbufferMembersData->members[m].matrixStride = typeReflectionData->MatrixStride;
                            cbufferData->cbufferMembersData->members[m].arrayStride = typeReflectionData->ArrayStride;
                            delete typeReflectionData;
                        }
                    }
                }
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

#ifdef XKSLANG_DEBUG_MODE
    //some sanity check in debug mode
    if (success)
    {
        for (unsigned int i = 0; i < listAllShaderCBuffers.size(); i++)
        {
            CBufferTypeData* cbufferData = listAllShaderCBuffers[i];
            if (cbufferData->cbufferMembersData != nullptr)
            {
                if (cbufferData->cbufferCountMembers != cbufferData->cbufferMembersData->countMembers()) {error("inconsistent number of members");}
                for (unsigned int m = 0; m < cbufferData->cbufferMembersData->countMembers(); ++m)
                {
                    const TypeStructMember& member = cbufferData->cbufferMembersData->members[m];

                    if (member.memberType == nullptr) { error("undefined member type for a cbuffer member"); }
                    if (member.memberTypeId == spvUndefinedId) { error("undefined member type id for a cbuffer member"); }
                    if (member.memberSize < 0) { error("undefined size for a cbuffer member"); }
                    if (!IsPow2(member.memberAlignment)) { error("invalid member alignment"); }
                    if (IsMatrixType(member.memberType) || IsMatrixArrayType(member.memberType)) {
                        if (member.matrixLayoutDecoration == -1) { error("undefined matrix member layout"); }
                        if (member.matrixStride == 0) { error("undefined matrix stride"); }
                    }
                    else {
                        if (member.matrixLayoutDecoration != -1) { error("got a defined matrix layout for a non-matrix member"); }
                        if (member.matrixStride != 0) { error("got a defined matrix stride for a non-matrix member"); }
                    }
                    if (IsArrayType(member.memberType))
                    {
                        //It could happen if someday we change type layout settings (to be different from glslang layouts), but we should then update all arrayStride decorate)
                        if (member.memberType->arrayStride != member.arrayStride) { error("inconsistent arrayStride for a cbuffer member type"); }
                    }
                }
            }
        }
        if (errorMessages.size() > 0) success = false;
    }
#endif

    spv::Id newBoundId = bound();

    //=========================================================================================================================
    //Stuff necessary to insert new bytecode
    BytecodeUpdateController bytecodeUpdateController;
    BytecodeChunk* bytecodeNewNamesAndDecocates = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, posLatestMemberNameOrDecorate, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);
    if (bytecodeNewNamesAndDecocates == nullptr) { error("Failed to insert a new bytecode chunk to insert new names and decorates"); success = false; }
    BytecodeChunk* bytecodeNewTypes = nullptr;
    unsigned int posToInsertNewTypes = 0;

    spv::Id idOpTypeVectorFloat4 = spvUndefinedId;  //id of the OpType: float[4]
    unsigned int maxConstValueNeeded = 0;
    vector<spv::Id> mapIndexesWithConstValueId; //map const value with their typeId
    //vector<CBufferTypeData*> listUntouchedCbuffers;   //this list will contain all cbuffers being kept as they are
    vector<TypeStructMemberArray*> listNewCbuffers;     //this list will contain all new cbuffer information
    vector<TypeStructMember> listResourcesNewAccessVariables;     //this list will contain access variable for all resources moved out from the cbuffer

    vector<CBufferTypeData*>& vectorCbuffersToRemap = mapUsedCbuffers;  //just reusing an existing vector to avoid creating a new one...
    std::fill(vectorCbuffersToRemap.begin(), vectorCbuffersToRemap.end(), nullptr);

    if (success)
    {
        //=========================================================================================================================
        //=========================================================================================================================
        //get or create a float[4] type in the bytecode: this type will be needed to add some padding in some merged cbuffers
        spv::Id idOpTypeFloat32 = spvUndefinedId;

        unsigned int countObjects = (unsigned int)(listAllObjects.size());
        for (unsigned int iObj = 0; iObj < countObjects; iObj++)
        {
            ObjectInstructionBase* obj = listAllObjects[iObj];
            if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
            {
                if (obj->bytecodeStartPosition > posToInsertNewTypes) posToInsertNewTypes = obj->bytecodeStartPosition;

                switch (obj->opCode)
                {
                    case spv::OpTypeFloat:
                    {
                        int width = asLiteralValue(obj->bytecodeStartPosition + 2);
                        if (width == 32) {
                            if (idOpTypeFloat32 != spvUndefinedId) {
                                error("We found at least 2 type: OpTypeFloat 32"); //this could be fine, but we should consider this case in the following "case spv::OpTypeVector" then
                            }
                            idOpTypeFloat32 = obj->GetId();
                        }
                        break;
                    }

                    case spv::OpTypeVector:
                    {
                        spv::Id vectorType = asId(obj->bytecodeStartPosition + 2);
                        int vectorSize = asLiteralValue(obj->bytecodeStartPosition + 3);
                        if (vectorSize == 4 && vectorType == idOpTypeFloat32) {
                            if (idOpTypeVectorFloat4 != spvUndefinedId) {
                                error("We found at least 2 type: OpTypeVector float32[4]"); //this could be fine, but we should consider this case in the following "case spv::OpTypeVector" then
                            }
                            idOpTypeVectorFloat4 = obj->GetId();
                        }
                        break;
                    }
                }
            }
        }

        if (idOpTypeVectorFloat4 == spvUndefinedId)
        {
            //create the float and float[4] types
            if (bytecodeNewTypes == nullptr)
                bytecodeNewTypes = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, posToInsertNewTypes, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);

            if (bytecodeNewTypes == nullptr)
            {
                error("Failed to create a new bytecode chunk to insert new types");
            }
            else
            {
                if (idOpTypeFloat32 == spvUndefinedId)
                {
                    spv::Instruction typeFloat32(newBoundId++, spv::NoType, spv::OpTypeFloat);
                    typeFloat32.addImmediateOperand(32);
                    typeFloat32.dump(bytecodeNewTypes->bytecode);
                    idOpTypeFloat32 = typeFloat32.getResultId();
                }

                spv::Instruction typeVectorFloat4(newBoundId++, spv::NoType, spv::OpTypeVector);
                typeVectorFloat4.addIdOperand(idOpTypeFloat32);
                typeVectorFloat4.addImmediateOperand(4);
                typeVectorFloat4.dump(bytecodeNewTypes->bytecode);
                idOpTypeVectorFloat4 = typeVectorFloat4.getResultId();
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

    if (success && anyCBufferUsed)
    {
        //=========================================================================================================================
        //=========================================================================================================================
        //merge all USED cbuffers having an undefined name, or sharing the same declaration name
        for (unsigned int i = 0; i < listAllShaderCBuffers.size(); i++)
        {
            CBufferTypeData* cbufferA = listAllShaderCBuffers[i];

            if (cbufferA->isUsed == false) continue; //cbuffer A is not used
            bool mergingUndefinedCbuffers = (cbufferA->isDefine == false);

            vector<CBufferTypeData*> someCBuffersToMerge;
            someCBuffersToMerge.push_back(cbufferA); //for consistency purpose, we will recreate every used cbuffers, even if it's not merged with another one

            //==========================================================================================
            //check if we find some USED cbuffers which have to be merged (all defined cbuffer sharing the same name, or all global cbuffer (undefined))
            for (unsigned int j = i + 1; j < listAllShaderCBuffers.size(); j++)
            {
                CBufferTypeData* cbufferB = listAllShaderCBuffers[j];

                if (cbufferB->isUsed == false) continue; //cbuffer B is not used
                bool isBUndefinedCbuffer = (cbufferB->isDefine == false);

                if (mergingUndefinedCbuffers)
                {
                    //merge all undefined cbuffers
                    //if (someCBuffersToMerge.size() == 0) someCBuffersToMerge.push_back(cbufferA);
                    if (isBUndefinedCbuffer) someCBuffersToMerge.push_back(cbufferB);
                }
                else
                {
                    //merge defined cbuffers, only if they share the same name
                    if (!isBUndefinedCbuffer && cbufferA->cbufferName == cbufferB->cbufferName)
                    {
                        //if (someCBuffersToMerge.size() == 0) someCBuffersToMerge.push_back(cbufferA);
                        someCBuffersToMerge.push_back(cbufferB);
                    }
                }
            }

            //==========================================================================================
            //merge the cbuffers
            int countCbuffersToMerge = (int)someCBuffersToMerge.size();
            if (countCbuffersToMerge > 0)
            {
                bool onlyMergeUsedMembers = (mergingUndefinedCbuffers? true: false); //either we merge the whole cbuffer (for defined cbuffers), or only the members used (for undefined cbuffers)
                bool canAddPaddingBeforeAndAfterCbufferWithSubpart = false;
                bool defaultPaddingAlreadyAdded = false;

                if (mergingUndefinedCbuffers == false)
                {
                    canAddPaddingBeforeAndAfterCbufferWithSubpart = true;

                    //we regroup the cbuffers depending on their subpart name (cbuffers with subpart name move at the end, plus we regroup those with same subpart name)
                    vector<CBufferTypeData*> cbufferWithSubParts;
                    vector<CBufferTypeData*> cbufferWithoutSubParts;
                    for (int k = 0; k < countCbuffersToMerge; k++)
                    {
                        CBufferTypeData* aCbuffer = someCBuffersToMerge[k];
                        if (aCbuffer->hasSubpartName()) cbufferWithSubParts.push_back(aCbuffer);
                        else cbufferWithoutSubParts.push_back(aCbuffer);
                    }

                    int countCbWithSubParts = (int)cbufferWithSubParts.size();
                    int countCbWithoutSubParts = (int)cbufferWithoutSubParts.size();
                    if (countCbWithSubParts > 0)
                    {
                        //regroup the cbuffers having a similar subpart name
                        for (int k1 = countCbWithSubParts - 1; k1 >= 0; k1--)
                        {
                            const string& subPartName = cbufferWithSubParts[k1]->cbufferSubpartName;
                            int posToInsert = -1;
                            for (int k2 = k1 - 1; k2 >= 0; k2--)
                            {
                                if (cbufferWithSubParts[k2]->cbufferSubpartName == subPartName) {
                                    if (posToInsert == -1) {
                                        //both cbuffers are already next to each other: do nothing
                                    }
                                    else {
                                        //swap the buffers
                                        CBufferTypeData* tmpCbuffer = cbufferWithSubParts[posToInsert];
                                        cbufferWithSubParts[posToInsert] = cbufferWithSubParts[k2];
                                        cbufferWithSubParts[k2] = tmpCbuffer;

                                        k2 = posToInsert;
                                        posToInsert = -1;
                                    }
                                }
                                else {
                                    if (posToInsert == -1) posToInsert = k2;
                                }
                            }
                        }

                        //For members belonging to a subpart cbuffer: we set their logicalGroup name
                        for (int k = 0; k < countCbWithSubParts; k++)
                        {
                            CBufferTypeData* aCbufferWithSubpart = cbufferWithSubParts[k];
                            for (int m = 0; m < aCbufferWithSubpart->cbufferCountMembers; m++)
                            {
                                aCbufferWithSubpart->cbufferMembersData->members[m].logicalGroup = aCbufferWithSubpart->cbufferSubpartName;
                            }
                        }

                        //reshuffle the cbuffers
                        int index = 0;
                        for (int k = 0; k < countCbWithoutSubParts; k++) someCBuffersToMerge[index++] = cbufferWithoutSubParts[k];
                        for (int k = 0; k < countCbWithSubParts; k++) someCBuffersToMerge[index++] = cbufferWithSubParts[k];
                    }
                }

                if (onlyMergeUsedMembers)
                {
                    //flag all Variables accessing a cbuffer that we're merging
                    {
                        for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it) {
                            ObjectInstructionBase* obj = *it;
                            if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Variable) {
                                VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
                                variable->tmpFlag = 0;
                            }
                        }
                        for (auto itcb = someCBuffersToMerge.begin(); itcb != someCBuffersToMerge.end(); itcb++){
                            CBufferTypeData* cbufferToMerge = *itcb;
                            cbufferToMerge->correspondingShaderType->variable->tmpFlag = 1;
                            for (unsigned int m = 0; m < cbufferToMerge->cbufferMembersData->members.size(); m++) cbufferToMerge->cbufferMembersData->members[m].isUsed = false;
                        }
                    }

                    //Detect which members from the cbuffers to merge are used
                    unsigned int start = positionFirstOpFunctionInstruction;
                    const unsigned int end = (unsigned int)spv.size();
                    while (start < end)
                    {
                        unsigned int wordCount = asWordCount(start);
                        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
                        if (wordCount == 0) { error("Corrupted bytecode: wordCount is equals to 0"); break; }
#endif

                        switch (opCode)
                        {
                            case spv::OpAccessChain:
                            {
                                spv::Id structIdAccessed = asId(start + 3);

                                VariableInstruction* variable = GetVariableById(structIdAccessed);
                                //if (variable == nullptr) { error("Failed to find the variable object for id: " + to_string(structIdAccessed)); break; }

                                //are we accessing the a cbuffer variable
                                if (variable != nullptr && variable->tmpFlag == 1)
                                {
                                    //spv::Id resultId = asId(start + 2);
                                    spv::Id indexConstId = asId(start + 4);

                                    ConstInstruction* constObject = GetConstById(indexConstId);
                                    if (constObject == nullptr) { error("cannot get const object for Id: " + to_string(indexConstId)); break; }
                                    int memberIndexValue = constObject->valueS32;
#ifdef XKSLANG_DEBUG_MODE
                                    if (variable->shaderOwner == nullptr) { error("the accessed variable does not belong to a shader: " + variable->GetName()); break; }
#endif
                                    ShaderTypeData* cbufferToMerge = variable->shaderOwner->GetShaderTypeDataForVariable(variable);
                                    CBufferTypeData* cbufferData = cbufferToMerge->type->cbufferData;
#ifdef XKSLANG_DEBUG_MODE
                                    if (cbufferToMerge == nullptr) { error("cannot get the cbuffer shader type for the variable: " + variable->GetName()); break; }
                                    if (cbufferData->cbufferMembersData == nullptr) { error("the cbuffer members data have not been initialized for: " + variable->GetName()); break; }
                                    if (memberIndexValue < 0 || memberIndexValue >= (int)cbufferData->cbufferMembersData->members.size()) {error("the member access index is out of bound"); break;}
#endif
                                    cbufferData->cbufferMembersData->members[memberIndexValue].isUsed = true;
                                }

                                break;
                            }
                        }
                        start += wordCount;
                    }

                    if (errorMessages.size() > 0) { success = false; break; }
                } //end of if (onlyMergeUsedMembers)
                
                TypeStructMemberArray* combinedCbuffer = new TypeStructMemberArray();
                combinedCbuffer->structTypeId = newBoundId++;
                combinedCbuffer->structPointerTypeId = newBoundId++;
                combinedCbuffer->structVariableTypeId = newBoundId++;

                //name of the combined cbuffer is the name of the first cbuffer (all merged cbuffers have the same name)
                combinedCbuffer->cbufferDeclarationName = someCBuffersToMerge[0]->cbufferName;

                //==========================================================================================
                //create the list with all members from the cbuffers we're merging
                for (int iCbufferBeingMerged = 0; iCbufferBeingMerged < countCbuffersToMerge; iCbufferBeingMerged++)
                {
                    CBufferTypeData* cbufferToMerge = someCBuffersToMerge[iCbufferBeingMerged];
                    ShaderClassData* cbufferShaderOwner = cbufferToMerge->shaderOwner;
                    const string cbufferToMergeShaderOwnerOriginalBaseName = cbufferShaderOwner->GetShaderOriginalBaseName();
                    ShaderTypeData* cbufferShaderType = cbufferToMerge->correspondingShaderType;

                    vectorCbuffersToRemap[cbufferShaderType->variable->GetId()] = cbufferToMerge; //store the cbuffer to remap
                    bool isMemberStaged = cbufferToMerge->isStage;

                    const unsigned int countMembersInBufferToMerge = (unsigned int)cbufferToMerge->cbufferMembersData->members.size();
                    for (unsigned int m = 0; m < countMembersInBufferToMerge; m++)
                    {
                        bool isFirstCbufferMember = (m == 0? true: false);
                        bool isLastCbufferMember = (m == (countMembersInBufferToMerge - 1) ? true : false);

                        TypeStructMember& memberToMerge = cbufferToMerge->cbufferMembersData->members[m];
                        if (onlyMergeUsedMembers && memberToMerge.isUsed == false){
                            continue;
                        }

                        if (memberToMerge.isResourceType)
                        {
                            //a resource won't be merged into a cbuffer, but moved out into the global space
                            memberToMerge.newStructMemberIndex = -1;
                            memberToMerge.newStructTypeId = 0;
                            memberToMerge.newStructVariableAccessTypeId = 0;
                            memberToMerge.isStage = isMemberStaged;

                            // We set the resource as: shaderOwnerName.originalName (this will be its id keyName)
                            memberToMerge.declarationName = cbufferToMerge->shaderOwner->GetShaderOriginalBaseName() + "." + memberToMerge.declarationName;

                            //We check if the resource has to be merge with an existing one
                            int mergeResourceWithResourceId = -1;
                            for (unsigned int pr = 0; pr < (unsigned int)listResourcesNewAccessVariables.size(); pr++)
                            {
                                TypeStructMember& anotherResource = listResourcesNewAccessVariables[pr];
                                if (memberToMerge.declarationName == anotherResource.declarationName)
                                {
                                    //check that they have the stage qualifier
                                    if (!memberToMerge.isStage || !anotherResource.isStage) {
                                        error("The resource: " + memberToMerge.declarationName + " is found several times but is not declared with stage qualifier");
                                        break;
                                    }

                                    //check that the 2 resources have the same type
                                    if (memberToMerge.memberType != anotherResource.memberType) {
                                        error("2 resources have an identical name but a different type: " + memberToMerge.declarationName);
                                        break;
                                    }

                                    mergeResourceWithResourceId = anotherResource.variableAccessTypeId;
                                    break;
                                }
                            }

                            if (mergeResourceWithResourceId != -1)
                            {
                                memberToMerge.variableAccessTypeId = mergeResourceWithResourceId;
                                memberToMerge.isResourceMergedWithAnotherMember = true;
                            }
                            else
                            {
                                if (cbufferToMerge->isDefine) memberToMerge.resourceGroupName = cbufferToMerge->cbufferName;
                                memberToMerge.variableAccessTypeId = newBoundId++; //id of the new variable we'll create
                                memberToMerge.isResourceMergedWithAnotherMember = false;
                            }

                            listResourcesNewAccessVariables.push_back(memberToMerge);
                        }
                        else
                        {
                            if (canAddPaddingBeforeAndAfterCbufferWithSubpart && isFirstCbufferMember && cbufferToMerge->hasSubpartName())
                            {
                                //we have a cbuffer with a subpart name: add the paddings
                                if (!defaultPaddingAlreadyAdded)
                                {
                                    defaultPaddingAlreadyAdded = true;

                                    string paddingMemberName = "_padding_" + combinedCbuffer->cbufferDeclarationName + "_Default";
                                    int memberIndex = (unsigned int)combinedCbuffer->members.size();
                                    combinedCbuffer->members.push_back(TypeStructMember());
                                    TypeStructMember& defaultPaddingStructMember = combinedCbuffer->members.back();
                                    defaultPaddingStructMember.declarationName = paddingMemberName;
                                    defaultPaddingStructMember.linkName = paddingMemberName;
                                    defaultPaddingStructMember.logicalGroup = "Default";
                                    defaultPaddingStructMember.isStage = false;
                                    defaultPaddingStructMember.cbufferShaderOwner = cbufferShaderOwner;
                                    defaultPaddingStructMember.memberTypeId = idOpTypeVectorFloat4;
                                    defaultPaddingStructMember.structMemberIndex = memberIndex;

                                    TypeReflectionDescription typeReflectionData;
                                    if (!GetTypeFloatVectorReflectionDescription(32, 4, typeReflectionData)) {
                                        error("Failed to get the float type reflection data"); break;
                                    }

                                    defaultPaddingStructMember.memberSize = typeReflectionData.Size;
                                    defaultPaddingStructMember.memberAlignment = typeReflectionData.Alignment;

                                    //compute the member offset
                                    int memberOffset = 0;
                                    if (memberIndex > 0)
                                    {
                                        int previousMemberOffset = combinedCbuffer->members[memberIndex - 1].memberOffset;
                                        int previousMemberSize = combinedCbuffer->members[memberIndex - 1].memberSize;
                                        memberOffset = previousMemberOffset + previousMemberSize;
                                        int memberAlignment = defaultPaddingStructMember.memberAlignment;
                                        memberOffset = (memberOffset + memberAlignment - 1) & (~(memberAlignment - 1)); //round to pow2
                                    }
                                    defaultPaddingStructMember.memberOffset = memberOffset;
                                }
                            }

                            int memberNewIndex = -1;
                            bool memberAlreadyAdded = false;
                            if (isMemberStaged)
                            {
                                for (unsigned int pm = 0; pm < combinedCbuffer->members.size(); pm++)
                                {
                                    TypeStructMember& anotherMember = combinedCbuffer->members[pm];
                                    if (anotherMember.isStage)
                                    {
                                        //We merge the members if they have the same declaration name, and the same base shader owner
                                        if (anotherMember.cbufferShaderOwner->GetShaderOriginalBaseName() == cbufferToMergeShaderOwnerOriginalBaseName &&
                                            anotherMember.declarationName == memberToMerge.declarationName)
                                        {
                                            //unless they have been set with a different linkName
                                            if (anotherMember.linkName == memberToMerge.linkName)
                                            {
                                                //check that they have they same type
                                                if (AreTypeInstructionsIdentical(anotherMember.memberTypeId, memberToMerge.memberTypeId))
                                                {
                                                    memberAlreadyAdded = true;
                                                    memberNewIndex = pm;
                                                    break;
                                                }
                                                else
                                                {
                                                    error("2 cbuffer members share a same cbuffer, same declaration name, and same link name, yet they have a different type: " + memberToMerge.declarationName);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            if (memberAlreadyAdded)
                            {
                                //member already added, nothing else to do
                            }
                            else
                            {
                                memberNewIndex = (unsigned int)combinedCbuffer->members.size();

                                combinedCbuffer->members.push_back(TypeStructMember(memberToMerge));
                                TypeStructMember& newMember = combinedCbuffer->members.back();

                                newMember.structMemberIndex = memberNewIndex;
                                newMember.isStage = isMemberStaged;
                                newMember.cbufferShaderOwner = cbufferShaderOwner;
                                
                                //compute the member offset (depending on the previous member's offset, its size, plus the new member's alignment
                                int memberOffset = 0;
                                if (memberNewIndex > 0)
                                {
                                    int previousMemberOffset = combinedCbuffer->members[memberNewIndex - 1].memberOffset;
                                    int previousMemberSize = combinedCbuffer->members[memberNewIndex - 1].memberSize;
                                    memberOffset = previousMemberOffset + previousMemberSize;
                                    int memberAlignment = newMember.memberAlignment;
                                    memberOffset = (memberOffset + memberAlignment - 1) & (~(memberAlignment - 1)); //round to pow2
                                }
                                newMember.memberOffset = memberOffset;
                            }

                            //link reference from previous member to new one
                            memberToMerge.newStructMemberIndex = memberNewIndex;
                            memberToMerge.newStructTypeId = combinedCbuffer->structTypeId;
                            memberToMerge.newStructVariableAccessTypeId = combinedCbuffer->structVariableTypeId;

                            if (canAddPaddingBeforeAndAfterCbufferWithSubpart && isLastCbufferMember && cbufferToMerge->hasSubpartName())
                            {
                                //we add a padding, unless the next cbuffer that we're going to merge has the same subpart name
                                bool canAddEndingPadding = true;
                                if (iCbufferBeingMerged < countCbuffersToMerge - 1)
                                {
                                    CBufferTypeData* nextCbufferToMerge = someCBuffersToMerge[iCbufferBeingMerged + 1];
                                    if (nextCbufferToMerge->cbufferSubpartName == cbufferToMerge->cbufferSubpartName) canAddEndingPadding = false;
                                }

                                if (canAddEndingPadding)
                                {
                                    //we have a cbuffer with a subpart name: add the padding at the end
                                    string paddingMemberName = "_padding_" + combinedCbuffer->cbufferDeclarationName + "_" + cbufferToMerge->cbufferSubpartName;
                                    int memberIndex = (unsigned int)combinedCbuffer->members.size();
                                    combinedCbuffer->members.push_back(TypeStructMember());
                                    TypeStructMember& defaultPaddingStructMember = combinedCbuffer->members.back();
                                    defaultPaddingStructMember.declarationName = paddingMemberName;
                                    defaultPaddingStructMember.linkName = paddingMemberName;
                                    defaultPaddingStructMember.logicalGroup = cbufferToMerge->cbufferSubpartName;
                                    defaultPaddingStructMember.isStage = false;
                                    defaultPaddingStructMember.cbufferShaderOwner = cbufferShaderOwner;
                                    defaultPaddingStructMember.memberTypeId = idOpTypeVectorFloat4;
                                    defaultPaddingStructMember.structMemberIndex = memberIndex;

                                    TypeReflectionDescription typeReflectionData;
                                    if (!GetTypeFloatVectorReflectionDescription(32, 4, typeReflectionData)) {
                                        error("Failed to get the float type reflection data"); break;
                                    }

                                    defaultPaddingStructMember.memberSize = typeReflectionData.Size;
                                    defaultPaddingStructMember.memberAlignment = typeReflectionData.Alignment;

                                    //compute the member offset
                                    int memberOffset = 0;
                                    if (memberIndex > 0)
                                    {
                                        int previousMemberOffset = combinedCbuffer->members[memberIndex - 1].memberOffset;
                                        int previousMemberSize = combinedCbuffer->members[memberIndex - 1].memberSize;
                                        memberOffset = previousMemberOffset + previousMemberSize;
                                        int memberAlignment = defaultPaddingStructMember.memberAlignment;
                                        memberOffset = (memberOffset + memberAlignment - 1) & (~(memberAlignment - 1)); //round to pow2
                                    }
                                    defaultPaddingStructMember.memberOffset = memberOffset;
                                }
                            }
                        }
                    }
                }

                if (combinedCbuffer->members.size() == 0)
                {
                    delete combinedCbuffer;
                    combinedCbuffer = nullptr;
                }
                else
                {
                    listNewCbuffers.push_back(combinedCbuffer);
                    if ((unsigned int)combinedCbuffer->members.size() > maxConstValueNeeded) maxConstValueNeeded = (unsigned int)combinedCbuffer->members.size();
                }
                
                //all cbuffers merged will be removed
                for (auto itcb = someCBuffersToMerge.begin(); itcb != someCBuffersToMerge.end(); itcb++)
                {
                    CBufferTypeData* cbufferToMerge = *itcb;
                    cbufferToMerge->isUsed = false;
                }
            }  //end of if (listCBuffersToMerge.size() > 0)
            else
            {
                //the cbuffer is used but not merged with anyone else: we still add it in this list, so that we can just rename it (for a cleaner bytecode)
                //listUntouchedCbuffers.push_back(cbufferA);
                error("not valid anymore");
            }
        } //end of for (unsigned int i = 0; i < listAllShaderCBuffers.size(); i++) (checking all used cbuffers)

        if (errorMessages.size() > 0) success = false;
    }

    if (success)
    {
        //===================================================================================================================
        //===================================================================================================================
        //get or build the maps of const values for the new cbuffers we're adding
        if (maxConstValueNeeded > 0)
        {
            //Build the map of const value with the existing const object Id, for each index of the global stream struct
            mapIndexesWithConstValueId.resize(maxConstValueNeeded, spvUndefinedId);
            spv::Id idOpTypeIntS32 = spvUndefinedId;
            unsigned int posLastConst = 0;

            for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
            {
                ObjectInstructionBase* obj = *it;
                if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Const)
                {
                    ConstInstruction* constObject = dynamic_cast<ConstInstruction*>(obj);
                    if (constObject->bytecodeStartPosition > posLastConst) posLastConst = constObject->bytecodeStartPosition;

                    if (constObject->isS32)
                    {
                        int constValueS32 = constObject->valueS32;
                        if (constValueS32 >= 0 && constValueS32 < (int)mapIndexesWithConstValueId.size()) mapIndexesWithConstValueId[constValueS32] = constObject->GetId();
                        if (idOpTypeIntS32 == spvUndefinedId) idOpTypeIntS32 = constObject->GetTypeId();
                    }
                }
            }

            BytecodeChunk* bytecodeNewConsts = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, posLastConst, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);
            if (bytecodeNewConsts == nullptr)
            {
                error("Failed to insert a new bytecode chunk to insert new consts");
            }
            else
            {
                //make the missing consts and OpTypeInt
                if (idOpTypeIntS32 == spvUndefinedId)
                {
                    spv::Instruction typeInt32(newBoundId++, spv::NoType, spv::OpTypeInt);
                    typeInt32.addImmediateOperand(32);
                    typeInt32.addImmediateOperand(1);
                    typeInt32.dump(bytecodeNewConsts->bytecode);
                    idOpTypeIntS32 = typeInt32.getResultId();
                }
                for (unsigned int i = 0; i < mapIndexesWithConstValueId.size(); ++i)
                {
                    if (mapIndexesWithConstValueId[i] == spvUndefinedId)
                    {
                        spv::Instruction constant(newBoundId++, idOpTypeIntS32, spv::OpConstant);
                        constant.addImmediateOperand(i);
                        constant.dump(bytecodeNewConsts->bytecode);
                        mapIndexesWithConstValueId[i] = constant.getResultId();
                    }
                }
            }

            if (errorMessages.size() > 0) success = false;
        }
    }

    //=============================================================================================================
    //=============================================================================================================
    // Naming: set the name to all cbuffer members
    if (success)
    {
        //get all shader instancing path data
        if (!GetAllShaderInstancingPathItems()) {
            error("Failed to get all shaders instancing path items");
        }
        else
        {
            map<string, bool> membersUsedRawName;
            map<string, bool> membersUsedKeyName;

            for (unsigned int icb = 0; icb < listNewCbuffers.size(); icb++)
            {
                TypeStructMemberArray* cbuffer = listNewCbuffers[icb];
                //string cbufferId = string("_id") + to_string(icb);

                //update the new member's name
                for (unsigned int pm = 0; pm < cbuffer->members.size(); pm++)
                {
                    TypeStructMember& aMember = cbuffer->members[pm];

                    ShaderClassData* shaderOwner = aMember.cbufferShaderOwner;
                    string shaderOriginalBaseName = shaderOwner->GetShaderOriginalBaseName();
                    string shaderFullNameWithoutGenerics = shaderOwner->GetShaderFullNameWithoutGenerics();
                    string shaderFullName = shaderOwner->GetShaderFullName();
                
                    const string memberOriginalDeclarationName = aMember.declarationName;

                    //==========================================
                    //string memberDeclarationName = (shaderOwner->countGenerics > 0? (shaderFullNameWithoutGenerics + cbufferId) : shaderFullNameWithoutGenerics) + "." + memberOriginalDeclarationName;
                    string memberDeclarationName = shaderFullNameWithoutGenerics + "." + memberOriginalDeclarationName;
                    aMember.declarationName = getRawNameFromKeyName(memberDeclarationName);

                    //==========================================
                    //member keyName
                    if (aMember.HasLinkName())
                    {
                        //nothing to do: the linkname is set by the user
                    }
                    else
                    {
                        if (aMember.isStage)
                        {
                            //stage member
                            aMember.linkName = shaderOriginalBaseName + "." + memberOriginalDeclarationName;
                        }
                        else
                        {
                            //unstage member (we use the shader original base name as well (even if this could make name conflicts))
                            aMember.linkName = shaderOriginalBaseName + "." + memberOriginalDeclarationName;

                            if (shaderOwner->listInstancingPathItems.size() > 0)
                            {
                                //shader has been instanciated through a composition: find back the compositions path to update its name
                                unsigned int countPaths = (unsigned int)(shaderOwner->listInstancingPathItems.size());
                                for (int pathLevel = countPaths - 1; pathLevel >= 0; pathLevel--)
                                {
                                    //look for the item matching the pathLevel
                                    const ShaderInstancingPathItem* instancingPathItem = nullptr;
                                    //for (int k = countPaths - 1; k >= 0; k--) //go backward because we're likely to have inserted the highest level at the beginning
                                    for (unsigned int k = 0; k < countPaths ; k++)
                                    {
                                        if (shaderOwner->listInstancingPathItems[k].instancePathLevel == pathLevel)
                                        {
                                            instancingPathItem = &(shaderOwner->listInstancingPathItems[k]);
                                            break;
                                        }
                                    }
                                    if (instancingPathItem == nullptr) {
                                        error("cannot find the shader instancing pathItem for level: " + to_string(pathLevel));
                                        break;
                                    }

                                    ShaderCompositionDeclaration* compositionInstantiated = GetCompositionDeclaration(instancingPathItem->compositionShaderOwnerId, instancingPathItem->compositionNum);
                                    if (compositionInstantiated == nullptr) {
                                        error("Composition not found for ShaderId: " + to_string(instancingPathItem->compositionShaderOwnerId) + " with composition num: " + to_string(instancingPathItem->compositionNum));
                                        break;
                                    }

                                    if (instancingPathItem->instanceNum >= compositionInstantiated->countInstances) {
                                        error("ShaderInstancingPathItem has an invalid instance num");
                                        break;
                                    }

                                    string suffix = "." + compositionInstantiated->variableName;
                                    if (compositionInstantiated->isArray) {
                                        suffix = suffix + "[" + to_string(instancingPathItem->instanceNum) + "]";
                                    }

                                    aMember.linkName = aMember.linkName + suffix;
                                }
                            }
                        }
                    }

                    //Check that the names are not conflicting
                    if (membersUsedRawName.find(aMember.declarationName) != membersUsedRawName.end())
                    {
                        error("2 cbuffer members have been assigned the same RawName: " + aMember.declarationName);
                        break;
                    }
                    membersUsedRawName[aMember.declarationName] = true;

                    if (membersUsedKeyName.find(aMember.linkName) != membersUsedKeyName.end())
                    {
                        error("2 cbuffer members have been assigned the same KeyName: " + aMember.linkName);
                        break;
                    }
                    membersUsedKeyName[aMember.linkName] = true;
                }

                if (errorMessages.size() > 0) break;
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

    if (success)
    {
        //===================================================================================================================
        //===================================================================================================================
        //Create the resources access variables
        for (auto itrav = listResourcesNewAccessVariables.begin(); itrav != listResourcesNewAccessVariables.end(); itrav++)
        {
            //for each resources: create the pointer type (if need), and the uniform access variable (to remove the cbuffer accessChain and directly load the variable)
            //example: a texture created on global space would be access through variable id (19)
            // 17:                    OpTypeImage 8(float)2D sampled format : Unknown
            // 18:                    OpTypePointer UniformConstant 17
            // 19(Texture0) : 18(ptr) OpVariable UniformConstant
            //....
            // 20:            17      OpLoad 19(Texture0)
            //While the same texture created within a cbuffer is accessed this way:
            // 51:                    OpTypeImage 29(float)2D sampled format : Unknown
            // 53:                    OpTypePointer UniformConstant 51
            //....
            // 10:            53(ptr) OpAccessChain 57(ResourceGroup_var) 54
            // 11:            51      OpLoad 10

            TypeStructMember& memberToMoveOut = *itrav;
            if (memberToMoveOut.isResourceMergedWithAnotherMember) continue;

            TypeInstruction* resourceType = memberToMoveOut.memberType;
            TypeInstruction* resourcePointerType = GetTypePointingTo(resourceType);
            VariableInstruction* resourceVariable = GetVariablePointingTo(resourcePointerType);
            if (resourceVariable != nullptr) { error("a variable already exists for the resource type"); break; }

            BytecodeChunk* bytecodeResourceVariable = nullptr;

            spv::Id pointerTypeId = 0;
            if (resourcePointerType == nullptr)
            {
                bytecodeResourceVariable = GetOrCreateNewBytecodeChunckToInsert(bytecodeUpdateController, resourceType->bytecodeStartPosition, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);
                if (bytecodeResourceVariable == nullptr) { error("Failed to insert a new bytecode chunk to create a resource pointer type"); break; }

                //make the pointer type
                pointerTypeId = newBoundId++;
                spv::Instruction pointer(pointerTypeId, spv::NoType, spv::OpTypePointer);
                pointer.addImmediateOperand(spv::StorageClass::StorageClassUniformConstant);
                pointer.addIdOperand(resourceType->GetId());
                pointer.dump(bytecodeResourceVariable->bytecode);
            }
            else
            {
                //check that the storage class is valid (Note: otherwise we can also create a new one)
                pointerTypeId = resourcePointerType->GetId();
                spv::StorageClass pointerStorageClass = (spv::StorageClass)asLiteralValue(resourcePointerType->GetBytecodeStartPosition() + 2);
                if (pointerStorageClass != spv::StorageClass::StorageClassUniformConstant) { error("Invalid storage class, expected StorageClassUniform"); break; }

                bytecodeResourceVariable = GetOrCreateNewBytecodeChunckToInsert(bytecodeUpdateController, resourcePointerType->bytecodeStartPosition, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);
                if (bytecodeResourceVariable == nullptr) { error("Failed to insert a new bytecode chunk to create a resource variable"); break; }
            }

            //make the variable
            {
                spv::Instruction variable(memberToMoveOut.variableAccessTypeId, pointerTypeId, spv::OpVariable);
                variable.addImmediateOperand(spv::StorageClass::StorageClassUniformConstant);
                variable.dump(bytecodeResourceVariable->bytecode);

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                string variableName = memberToMoveOut.declarationName;
                spv::Instruction variableNameInstr(spv::OpName);
                variableNameInstr.addIdOperand(variable.getResultId());
                variableNameInstr.addStringOperand(variableName.c_str());
                variableNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
#endif
                
                //add the variable's descriptorSet 0
                spv::Instruction variableDecorateInstr(spv::OpDecorate);
                variableDecorateInstr.addIdOperand(memberToMoveOut.variableAccessTypeId);
                variableDecorateInstr.addImmediateOperand(spv::DecorationDescriptorSet);
                variableDecorateInstr.addImmediateOperand(0);
                variableDecorateInstr.dump(bytecodeNewNamesAndDecocates->bytecode);

                //variable linkName (if any)
                if (memberToMoveOut.HasLinkName())
                {
                    spv::Instruction memberNameInstr(spv::OpLinkName);
                    memberNameInstr.addIdOperand(variable.getResultId());
                    memberNameInstr.addStringOperand(memberToMoveOut.linkName.c_str());
                    memberNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                //variable samplerState desc
                if (memberToMoveOut.HasSamplerStateDesc())
                {
                    uint32_t samplerStateDescPos = memberToMoveOut.samplerStateDestBytecodePos;

                    if (samplerStateDescPos >= spv.size()) { error("Invalid sampler state desc bytecode position"); break; }
                    spv::Op op = asOpCode(samplerStateDescPos);
                    int words = asWordCount(samplerStateDescPos);

                    if (op != spv::Op::OpMemberSamplerStateDef || words <= 3) { error("Invalid sampler state desc OpCode"); break; }

                    spv::Instruction samplerStateDesc(spv::OpSamplerStateDef);
                    samplerStateDesc.addIdOperand(variable.getResultId());
                    for (int ops = 3; ops < words; ops++)
                        samplerStateDesc.addImmediateOperand(spv[samplerStateDescPos + ops]);
                    samplerStateDesc.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                //variable resourceGroupName (if any)
                if (memberToMoveOut.HasResourceGroupName())
                {
                    spv::Instruction memberResourceGroupNameInstr(spv::OpResourceGroupName);
                    memberResourceGroupNameInstr.addIdOperand(variable.getResultId());
                    memberResourceGroupNameInstr.addStringOperand(memberToMoveOut.resourceGroupName.c_str());
                    memberResourceGroupNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                //variable resourceGroupName (if any)
                if (memberToMoveOut.HasLogicalGroupName())
                {
                    spv::Instruction memberLogicalGroupNameInstr(spv::OpLogicalGroupName);
                    memberLogicalGroupNameInstr.addIdOperand(variable.getResultId());
                    memberLogicalGroupNameInstr.addStringOperand(memberToMoveOut.logicalGroup.c_str());
                    memberLogicalGroupNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }
            }
        }
        
        //===================================================================================================================
        //===================================================================================================================
        //Create the new cbuffer structs
        for (auto itcb = listNewCbuffers.begin(); itcb != listNewCbuffers.end(); itcb++)
        {
            TypeStructMemberArray* cbuffer = *itcb;
            string& cbufferName = cbuffer->cbufferDeclarationName;
            string cbufferVarName = cbufferName + "_var";

            if (bytecodeNewTypes == nullptr)
            {
                bytecodeNewTypes = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, posToInsertNewTypes, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);
                if (bytecodeNewTypes == nullptr) { error("Failed to create a new bytecode chunk to insert new types"); break; }
            }

            //make the cbuffer struct type
            {
                spv::Instruction cbufferType(cbuffer->structTypeId, spv::NoType, spv::OpTypeStruct);
                for (unsigned int m = 0; m < cbuffer->members.size(); ++m)
                {
                    const TypeStructMember& aMember = cbuffer->members[m];
                    cbufferType.addIdOperand(aMember.memberTypeId);
                }
                cbufferType.dump(bytecodeNewTypes->bytecode);

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                //cbuffer struct name
                spv::Instruction cbufferStructName(spv::OpName);
                cbufferStructName.addIdOperand(cbufferType.getResultId());
                cbufferStructName.addStringOperand(cbufferName.c_str());
                cbufferStructName.dump(bytecodeNewNamesAndDecocates->bytecode);
#endif

                //cbuffer struct decorate (block / cbuffer)
                spv::Instruction structDecorateInstr(spv::OpDecorate);
                structDecorateInstr.addIdOperand(cbuffer->structTypeId);
                structDecorateInstr.addImmediateOperand(spv::DecorationBlock);
                structDecorateInstr.dump(bytecodeNewNamesAndDecocates->bytecode);

                //cbuffer declaration name
                spv::Instruction cbufferDeclarationNameInstr(spv::OpDeclarationName);
                cbufferDeclarationNameInstr.addIdOperand(cbuffer->structTypeId);
                cbufferDeclarationNameInstr.addStringOperand(cbufferName.c_str());
                cbufferDeclarationNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);

                //cbuffer properties (block / cbuffer)
                spv::Instruction structCBufferPropertiesInstr(spv::OpCBufferProperties);
                structCBufferPropertiesInstr.addIdOperand(cbuffer->structTypeId);
                structCBufferPropertiesInstr.addImmediateOperand(spv::CBufferDefined);
                structCBufferPropertiesInstr.addImmediateOperand(spv::CBufferUnstage);
                structCBufferPropertiesInstr.addImmediateOperand(cbuffer->countMembers());
                structCBufferPropertiesInstr.addStringOperand("");  //no subpart name (they have been processed at this stage)

                //for (unsigned int m = 0; m < cbuffer->members.size(); ++m)
                //{
                //    //add size and alignment for each members
                //    structCBufferPropertiesInstr.addImmediateOperand(cbuffer->members[m].memberSize);
                //    structCBufferPropertiesInstr.addImmediateOperand(cbuffer->members[m].memberAlignment);
                //}
                structCBufferPropertiesInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
            }

            //make the pointer type
            {
                spv::Instruction pointer(cbuffer->structPointerTypeId, spv::NoType, spv::OpTypePointer);
                pointer.addImmediateOperand(spv::StorageClass::StorageClassUniform);
                pointer.addIdOperand(cbuffer->structTypeId);
                pointer.dump(bytecodeNewTypes->bytecode);
            }
            
            //make the variable
            {
                spv::Instruction variable(cbuffer->structVariableTypeId, cbuffer->structPointerTypeId, spv::OpVariable);
                variable.addImmediateOperand(spv::StorageClass::StorageClassUniform);
                variable.dump(bytecodeNewTypes->bytecode);

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                spv::Instruction variableName(spv::OpName);
                variableName.addIdOperand(variable.getResultId());
                variableName.addStringOperand(cbufferVarName.c_str());
                variableName.dump(bytecodeNewNamesAndDecocates->bytecode);
#endif
            }

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //Add the members name
            for (unsigned int memberIndex = 0; memberIndex < cbuffer->members.size(); ++memberIndex)
            {
                const TypeStructMember& cbufferMember = cbuffer->members[memberIndex];

                //member name
                spv::Instruction memberNameInstr(spv::OpMemberName);
                memberNameInstr.addIdOperand(cbuffer->structTypeId);
                memberNameInstr.addImmediateOperand(memberIndex);
                memberNameInstr.addStringOperand(cbufferMember.declarationName.c_str()); //use declaration name
                memberNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
            }
#endif

            //Add the members decorate
            for (unsigned int memberIndex = 0; memberIndex < cbuffer->members.size(); ++memberIndex)
            {
                const TypeStructMember& cbufferMember = cbuffer->members[memberIndex];

                if (cbufferMember.memberOffset < 0) {
                    error("An offset has not been set for the member: " + cbufferMember.GetDeclarationNameOrSemantic());
                    break;
                }

                //member attribute (if any)
                if (cbufferMember.HasAttribute())
                {
                    spv::Instruction memberNameInstr(spv::OpMemberAttribute);
                    memberNameInstr.addIdOperand(cbuffer->structTypeId);
                    memberNameInstr.addImmediateOperand(memberIndex);
                    memberNameInstr.addStringOperand(cbufferMember.attribute.c_str());
                    memberNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                //member linkName (if any)
                if (cbufferMember.HasLinkName())
                {
                    spv::Instruction memberNameInstr(spv::OpMemberLinkName);
                    memberNameInstr.addIdOperand(cbuffer->structTypeId);
                    memberNameInstr.addImmediateOperand(memberIndex);
                    memberNameInstr.addStringOperand(cbufferMember.linkName.c_str());
                    memberNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                if (cbufferMember.HasLogicalGroupName())
                {
                    spv::Instruction memberNameInstr(spv::OpMemberLogicalGroup);
                    memberNameInstr.addIdOperand(cbuffer->structTypeId);
                    memberNameInstr.addImmediateOperand(memberIndex);
                    memberNameInstr.addStringOperand(cbufferMember.logicalGroup.c_str());
                    memberNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                //member decorate: offset
                {
                    spv::Instruction memberOffsetDecorateInstr(spv::OpMemberDecorate);
                    memberOffsetDecorateInstr.addIdOperand(cbuffer->structTypeId);
                    memberOffsetDecorateInstr.addImmediateOperand(cbufferMember.structMemberIndex);
                    memberOffsetDecorateInstr.addImmediateOperand(spv::DecorationOffset);
                    memberOffsetDecorateInstr.addImmediateOperand(cbufferMember.memberOffset);
                    memberOffsetDecorateInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                //member decorate: matrix layout
                if (cbufferMember.matrixLayoutDecoration != -1)
                {
                    spv::Instruction memberOffsetDecorateInstr(spv::OpMemberDecorate);
                    memberOffsetDecorateInstr.addIdOperand(cbuffer->structTypeId);
                    memberOffsetDecorateInstr.addImmediateOperand(cbufferMember.structMemberIndex);
                    memberOffsetDecorateInstr.addImmediateOperand(cbufferMember.matrixLayoutDecoration);
                    memberOffsetDecorateInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                if (cbufferMember.matrixStride > 0)
                {
                    spv::Instruction memberMatrixStrideDecorateInstr(spv::OpMemberDecorate);
                    memberMatrixStrideDecorateInstr.addIdOperand(cbuffer->structTypeId);
                    memberMatrixStrideDecorateInstr.addImmediateOperand(cbufferMember.structMemberIndex);
                    memberMatrixStrideDecorateInstr.addImmediateOperand(spv::DecorationMatrixStride);
                    memberMatrixStrideDecorateInstr.addImmediateOperand(cbufferMember.matrixStride);
                    memberMatrixStrideDecorateInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                //member extra decorates
                /*if (cbufferMember.listMemberDecoration.size() > 0)
                {
                    unsigned int cur = 0;
                    while (cur < cbufferMember.listMemberDecoration.size())
                    {
                        unsigned int decorateCount = cbufferMember.listMemberDecoration[cur];
                        cur++;
#ifdef XKSLANG_DEBUG_MODE
                        if (cur + decorateCount > cbufferMember.listMemberDecoration.size()) { error("Invalid member decorate instructions"); break; }
#endif

                        spv::Instruction memberDecorateInstr(spv::OpMemberDecorate);
                        memberDecorateInstr.addIdOperand(cbuffer->structTypeId);
                        memberDecorateInstr.addImmediateOperand(cbufferMember.structMemberIndex);
                        for (unsigned int k = 0; k < decorateCount; k++)
                            memberDecorateInstr.addImmediateOperand(cbufferMember.listMemberDecoration[cur + k]);
                        memberDecorateInstr.dump(bytecodeNewNamesAndDecocates->bytecode);

                        cur += decorateCount;
                    }
                }*/
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

    if (success)
    {
        vector<spv::Id> mapAccessChainResultIdsToRemap;
        mapAccessChainResultIdsToRemap.resize(bound(), 0);
        bool anyAccessChainResultIdToRemap = false;

        //===================================================================================================================
        //===================================================================================================================
        //Update all accesses to previous cbuffer to the new ones
        if (listNewCbuffers.size() > 0 || listResourcesNewAccessVariables.size() > 0)
        {
            unsigned int start = positionFirstOpFunctionInstruction;
            const unsigned int end = (unsigned int)spv.size();
            while (start < end)
            {
                unsigned int wordCount = asWordCount(start);
                spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
                if (wordCount == 0) { error("Corrupted bytecode: wordCount is equals to 0"); break; }
#endif

                switch (opCode)
                {
                    case spv::OpAccessChain:
                    {
                        spv::Id structIdAccessed = asId(start + 3);

                        //are we accessing a cbuffer that we just merged?
                        if (vectorCbuffersToRemap[structIdAccessed] != nullptr)
                        {
                            CBufferTypeData* cbufferData = vectorCbuffersToRemap[structIdAccessed];
                            TypeStructMemberArray* cbufferMembersData = cbufferData->cbufferMembersData;

                            spv::Id typeId = asId(start + 1);
                            spv::Id resultId = asId(start + 2);
                            spv::Id indexConstId = asId(start + 4);

                            ConstInstruction* constObject = GetConstById(indexConstId);
                            if (constObject == nullptr) { error("cannot get const object for Id: " + to_string(indexConstId)); break; }
                            int memberIndexInOriginalCbuffer = constObject->valueS32;

#ifdef XKSLANG_DEBUG_MODE
                            if (!constObject->isS32) { error("const object is not a valid S32"); break; }
                            if (cbufferMembersData == nullptr) { error("the original shaderType cbuffer has not initialized its members"); break; }
                            if (memberIndexInOriginalCbuffer < 0 || memberIndexInOriginalCbuffer >= (int)cbufferMembersData->members.size()) { error("memberIndexInOriginalCbuffer is out of bound"); break; }
#endif
                            const TypeStructMember& structMember = cbufferMembersData->members[memberIndexInOriginalCbuffer];

                            if (structMember.isResourceType)
                            {
                                //the resource has been moved outside the cbuffer: we remove the access chain instruction
                                spv::Id accessChainResultId = asId(start + 2);
                                if (AddPortionToRemove(bytecodeUpdateController, start, wordCount) == nullptr) { error("Failed to insert a portion to remove"); break; }

                                //Then we will need to remap the access chain id into the resource variable id
                                mapAccessChainResultIdsToRemap[accessChainResultId] = structMember.variableAccessTypeId;
                                anyAccessChainResultIdToRemap = true;
                            }
                            else
                            {
                                spv::Id newStructAccessId = structMember.newStructVariableAccessTypeId;
                                int memberIndexInNewCbuffer = structMember.newStructMemberIndex;

                                //could eventually be optimized, but we shouldn't have too many new cbuffers to bother for now
                                TypeStructMemberArray* newCbuffer = nullptr;
                                for (unsigned int i = 0; i < listNewCbuffers.size(); ++i)
                                {
                                    if (listNewCbuffers[i]->structVariableTypeId == newStructAccessId) {
                                        newCbuffer = listNewCbuffers[i];
                                        break;
                                    }
                                }
                                if (newCbuffer == nullptr) { error("unable to find the new cbuffer"); break; }
#ifdef XKSLANG_DEBUG_MODE
                                if (memberIndexInNewCbuffer < 0 || memberIndexInNewCbuffer >= (int)mapIndexesWithConstValueId.size()) { error("memberIndexInNewCbuffer is out of bound"); break; }
#endif
                                spv::Id memberIndexInNewCbufferConstTypeId = mapIndexesWithConstValueId[memberIndexInNewCbuffer]; //get the const type id for new index

                                BytecodePortionToReplace* portionToReplace = SetNewPortionToReplace(bytecodeUpdateController, start + 1);
                                portionToReplace->SetNewValues({ typeId, resultId, newStructAccessId, memberIndexInNewCbufferConstTypeId });
                            }
                        }
                        break;
                    }
                }
                start += wordCount;
            }
        }
        if (errorMessages.size() > 0) success = false;

        if (success && anyAccessChainResultIdToRemap)
        {
            //Remap all OpLoad instruction refering to a resource we moved out from the cbuffer
            //Note: could we have any other kind of instruction?
            unsigned int start = positionFirstOpFunctionInstruction;
            const unsigned int end = (unsigned int)spv.size();
            while (start < end)
            {
                unsigned int wordCount = asWordCount(start);
                spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
                if (wordCount == 0) { error("Corrupted bytecode: wordCount is equals to 0"); break; }
#endif

                switch (opCode)
                {
                    case spv::OpLoad:
                    {
                        spv::Id idLoaded = asId(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (idLoaded >= mapAccessChainResultIdsToRemap.size()) { error("idLoaded is out of bound"); break; }
#endif
                        if (mapAccessChainResultIdsToRemap[idLoaded] != 0)
                        {
                            spv::Id newId = mapAccessChainResultIdsToRemap[idLoaded];
                            SetNewAtomicValueUpdate(bytecodeUpdateController, start + 3, newId);
                        }
                        break;
                    }
                }
                start += wordCount;
            }

            if (errorMessages.size() > 0) success = false;
        }
    }

    //=========================================================================================================================
    //delete allocated data
    for (unsigned int i = 0; i < listAllShaderCBuffers.size(); i++)
    {
        CBufferTypeData* cbufferData = listAllShaderCBuffers[i];
        if (cbufferData->cbufferMembersData != nullptr)
        {
            delete cbufferData->cbufferMembersData;
            cbufferData->cbufferMembersData = nullptr;
        }
    }
    for (auto itcb = listNewCbuffers.begin(); itcb != listNewCbuffers.end(); itcb++)
    {
        TypeStructMemberArray* cbuffer = *itcb;
        delete cbuffer;
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //apply all changes
    if (success)
    {
        //add the new cbuffers into the bytecode
        setBound(newBoundId);

        //apply the bytecode update controller
        if (!ApplyBytecodeUpdateController(bytecodeUpdateController)) error("failed to update the bytecode update controller");

        //bytecode has been updated: reupdate all maps
        if (!UpdateAllMaps()) error("failed to update all maps");

        if (errorMessages.size() > 0) success = false;
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //remove all unused cbuffers
    if (success)
    {
        vector<range_t> vecStripRanges;
        for (auto itcb = listAllShaderCBuffers.begin(); itcb != listAllShaderCBuffers.end(); itcb++)
        {
            CBufferTypeData* cbuffer = *itcb;
            if (cbuffer->isUsed == false)
            {
                if (!RemoveShaderTypeFromBytecodeAndData(cbuffer->correspondingShaderType, vecStripRanges))
                {
                    return error(string("Failed to remove the unused cbuffer type: ") + cbuffer->correspondingShaderType->type->GetName());
                }
            }
        }
    
        stripBytecode(vecStripRanges);
        if (!UpdateAllMaps()) return error("failed to update all maps");
    }

    if (errorMessages.size() > 0) return false;
    return true;
}
