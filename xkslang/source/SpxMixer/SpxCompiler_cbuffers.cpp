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
    vector<CBufferTypeData*> vectorUsedCbuffers;
    vectorUsedCbuffers.resize(bound(), nullptr);
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
                if (shaderType->type->GetId() >= vectorUsedCbuffers.size()) { error("cbuffer type id is out of bound. Id: " + to_string(shaderType->type->GetId())); break; }
                if (cbufferData->cbufferCountMembers <= 0) { error("invalid count members for cbuffer: " + shaderType->type->GetName()); break; }
#endif

                cbufferData->isUsed = true;
                if (vectorUsedCbuffers[shaderType->type->GetId()] == nullptr)
                {
                    cbufferData->cbufferMembersData = new TypeStructMemberArray();
                    cbufferData->cbufferMembersData->members.resize(cbufferData->cbufferCountMembers);

                    //set both type and variable IDs
                    vectorUsedCbuffers[shaderType->type->GetId()] = cbufferData;
                    vectorUsedCbuffers[shaderType->variable->GetId()] = cbufferData;
                }
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //Retrieve information from the bytecode for all USED cbuffers, and their members
    unsigned int posLatestMemberNameOrDecorate = header_size;
    unsigned int posFirstOpNameOrDecorate = (unsigned int)spv.size();
    if (success && anyCBufferUsed)
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpName:
                {
                    if (posFirstOpNameOrDecorate > start) posFirstOpNameOrDecorate = start;

                    spv::Id id = asId(start + 1);
                    if (vectorUsedCbuffers[id] != nullptr)
                    {
                        //ShaderTypeData* cbuffer = fdsfsdf;
                        CBufferTypeData* cbufferData = vectorUsedCbuffers[id];

                        if (start + wordCount > posLatestMemberNameOrDecorate) posLatestMemberNameOrDecorate = start + wordCount;
                        
                        if (cbufferData->correspondingShaderType->type->GetId() == id) cbufferData->posOpNameType = start;
                        else if (cbufferData->correspondingShaderType->variable->GetId() == id) cbufferData->posOpNameVariable = start;
                    }
                    break;
                }

                case spv::OpMemberAttribute:
                {
                    const spv::Id id = asId(start + 1);
                    if (vectorUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorUsedCbuffers[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string attribute = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != id) { error("Invalid instruction Id"); break; }
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].attribute = attribute;
                    }
                    break;
                }

                /*case spv::OpMemberProperties:
                {
                    const spv::Id id = asId(start + 1);
                    if (vectorUsedCbuffers[id] != nullptr)
                    {
                        const ShaderTypeData* cbuffer = vectorUsedCbuffers[id];
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
                    if (vectorUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorUsedCbuffers[id];

                        if (start + wordCount > posLatestMemberNameOrDecorate) posLatestMemberNameOrDecorate = start + wordCount;

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

                //we're not getting the size / alignment from this instruction anymore
                /*case spv::OpCBufferProperties:
                {
                    const spv::Id typeId = asId(start + 1);

                    if (vectorUsedCbuffers[typeId] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorUsedCbuffers[typeId];

                        if (start + wordCount > posLatestMemberNameOrDecorate) posLatestMemberNameOrDecorate = start + wordCount;

                        spv::XkslPropertyEnum cbufferType = (spv::XkslPropertyEnum)asLiteralValue(start + 2);
                        spv::XkslPropertyEnum cbufferStage = (spv::XkslPropertyEnum)asLiteralValue(start + 3);
                        unsigned int countMembers = asLiteralValue(start + 4);
                        unsigned int remainingBytes = wordCount - 5;

#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferData->correspondingShaderType->type->GetId() != typeId) { error("Invalid instruction Id"); break; }
                        if (cbufferType != cbufferData->cbufferType) { error("Invalid cbuffer type property"); break; }
                        if (countMembers != cbufferData->cbufferCountMembers) { error("Invalid cbuffer count members property"); break; }
#endif

                        if (countMembers != cbufferData->cbufferMembersData->countMembers()) { error("Inconsistent number of members"); break; }
                        if (remainingBytes != countMembers * 2) { error("OpCBufferProperties instruction has an invalid number of bytes"); break; }
                        
                        for (unsigned int m = 0; m < countMembers; m++)
                        {
                            unsigned int k = start + 5 + (m * 2);
                            unsigned int memberSize = asLiteralValue(k);
                            unsigned int memberAlignment = asLiteralValue(k + 1);

                            cbufferData->cbufferMembersData->members[m].memberSize = memberSize;
                            cbufferData->cbufferMembersData->members[m].memberAlignment = memberAlignment;
                        }
                    }
                    break;
                }*/

                case spv::OpMemberDecorate:
                {
                    if (posFirstOpNameOrDecorate > start) posFirstOpNameOrDecorate = start;

                    spv::Id id = asId(start + 1);
                    if (vectorUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorUsedCbuffers[id];
                        if (start + wordCount > posLatestMemberNameOrDecorate) posLatestMemberNameOrDecorate = start + wordCount;

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
                    if (vectorUsedCbuffers[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorUsedCbuffers[id];

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
        if (posFirstOpNameOrDecorate > posLatestMemberNameOrDecorate) posFirstOpNameOrDecorate = posLatestMemberNameOrDecorate;
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
                        if (cbufferMemberType == nullptr) return error("failed to find the cbuffer element type for id: " + to_string(cbufferMemberTypeId));

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
                            if (!GetTypeReflectionDescription(cbufferMemberType, isMatrixRowMajor, member.attribute, *typeReflectionData, startPositionOfAllMemberDecorateInstructions))
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

    //=========================================================================================================================
    //Stuff necessary to insert new bytecode
    BytecodeUpdateController bytecodeUpdateController;
    BytecodeChunk* bytecodeNewNamesAndDecocates = bytecodeUpdateController.InsertNewBytecodeChunckAt(posLatestMemberNameOrDecorate, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertFirst);
    spv::Id newBoundId = bound();

    unsigned int maxConstValueNeeded = 0;
    vector<spv::Id> mapIndexesWithConstValueId; //map const value with their typeId
    //vector<CBufferTypeData*> listUntouchedCbuffers;   //this list will contain all cbuffers being kept as they are
    vector<TypeStructMemberArray*> listNewCbuffers;     //this list will contain all new cbuffer information
    vector<TypeStructMember> listResourcesNewAccessVariables;     //this list will contain access variable for all resources moved out from the cbuffer

    vector<CBufferTypeData*>& vectorCbuffersToRemap = vectorUsedCbuffers;  //just reusing an existing vector to avoid creating a new one...
    std::fill(vectorCbuffersToRemap.begin(), vectorCbuffersToRemap.end(), nullptr);

    if (success && anyCBufferUsed)
    {
        //=========================================================================================================================
        //=========================================================================================================================
        //merge all USED cbuffers having an undefined type, or sharing the same declaration name
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
                    if (someCBuffersToMerge.size() == 0) someCBuffersToMerge.push_back(cbufferA);
                    if (isBUndefinedCbuffer) someCBuffersToMerge.push_back(cbufferB);
                }
                else
                {
                    //merge defined cbuffers, only if they share the same name
                    if (!isBUndefinedCbuffer && cbufferA->cbufferName == cbufferB->cbufferName)
                    {
                        if (someCBuffersToMerge.size() == 0) someCBuffersToMerge.push_back(cbufferA);
                        someCBuffersToMerge.push_back(cbufferB);
                    }
                }
            }

            //==========================================================================================
            //merge the cbuffers
            if (someCBuffersToMerge.size() > 0)
            {
                bool onlyMergeUsedMembers = (mergingUndefinedCbuffers? true: false); //either we merge the whole cbuffer (for defined cbuffers), or only the members used (for undefined cbuffers)

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
                combinedCbuffer->tmpTargetedBytecodePosition = 0;

                //==========================================================================================
                //create the list with all members from the cbuffers we're merging
                for (auto itcb = someCBuffersToMerge.begin(); itcb != someCBuffersToMerge.end(); itcb++)
                {
                    CBufferTypeData* cbufferToMerge = *itcb;
                    ShaderTypeData* cbufferShaderType = cbufferToMerge->correspondingShaderType;

                    vectorCbuffersToRemap[cbufferShaderType->variable->GetId()] = cbufferToMerge; //store the cbuffer to remap
                    bool isMemberStaged = cbufferToMerge->isStage;

                    //check the best position where to insert the new cbuffer in the bytecode
                    if (cbufferShaderType->type->bytecodeEndPosition > combinedCbuffer->tmpTargetedBytecodePosition) combinedCbuffer->tmpTargetedBytecodePosition = cbufferShaderType->type->bytecodeEndPosition;

                    const unsigned int countMembersInBufferToMerge = (unsigned int)cbufferToMerge->cbufferMembersData->members.size();
                    for (unsigned int m = 0; m < countMembersInBufferToMerge; m++)
                    {
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
                            memberToMerge.declarationName = cbufferToMerge->shaderOwnerName + "_" + memberToMerge.declarationName;
                            memberToMerge.variableAccessTypeId = newBoundId++; //id of the new variable we'll create

                            listResourcesNewAccessVariables.push_back(memberToMerge);
                        }
                        else
                        {
                            int memberNewIndex = -1;
                            bool memberAlreadyAdded = false;
                            if (isMemberStaged)
                            {
                                for (unsigned int pm = 0; pm < combinedCbuffer->members.size(); pm++)
                                {
                                    TypeStructMember& anotherMember = combinedCbuffer->members[pm];
                                    if (anotherMember.isStage)
                                    {
                                        if (anotherMember.shaderOwnerName == cbufferToMerge->shaderOwnerName && anotherMember.declarationName == memberToMerge.declarationName)
                                        {
                                            memberAlreadyAdded = true;
                                            memberNewIndex = pm;
                                            break;
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
                                //newMember.listMemberDecoration = memberToMerge.listMemberDecoration;
                                newMember.shaderOwnerName = cbufferToMerge->shaderOwnerName;
                        
                                //compute the member offset (depending on the previous member's offset, its size, plus the new member's alignment
                                int memberOffset = 0;
                                if (memberNewIndex > 0)
                                {
                                    int previousMemberOffset = combinedCbuffer->members[memberNewIndex - 1].memberOffset;
                                    int previousMemberSize = combinedCbuffer->members[memberNewIndex - 1].memberSize;

                                    memberOffset = previousMemberOffset + previousMemberSize;
                                    int memberAlignment = newMember.memberAlignment;
                                    //round to pow2
                                    memberOffset = (memberOffset + memberAlignment - 1) & (~(memberAlignment - 1));
                                }
                                newMember.memberOffset = memberOffset;
                            }

                            //link reference from previous member to new one
                            memberToMerge.newStructMemberIndex = memberNewIndex;
                            memberToMerge.newStructTypeId = combinedCbuffer->structTypeId;
                            memberToMerge.newStructVariableAccessTypeId = combinedCbuffer->structVariableTypeId;
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

                    //name of the combined cbuffer is the name of the first cbuffer (all merged cbuffers have the same name)
                    combinedCbuffer->declarationName = validateName(someCBuffersToMerge[0]->cbufferName);

                    //update the new member's name with a more explicit one (ex. var1 --> ShaderA.var1, if the member is stage we use the original shader name)
                    for (unsigned int pm = 0; pm < combinedCbuffer->members.size(); pm++)
                    {
                        TypeStructMember& anotherMember = combinedCbuffer->members[pm];
                        anotherMember.declarationName = validateName(anotherMember.shaderOwnerName + "_" + anotherMember.declarationName);
                    }
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
            unsigned int posLastConstEnd = 0;

            for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
            {
                ObjectInstructionBase* obj = *it;
                if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Const)
                {
                    ConstInstruction* constObject = dynamic_cast<ConstInstruction*>(obj);
                    if (constObject->bytecodeEndPosition > posLastConstEnd) posLastConstEnd = constObject->bytecodeEndPosition;

                    if (constObject->isS32)
                    {
                        int constValueS32 = constObject->valueS32;
                        if (constValueS32 >= 0 && constValueS32 < (int)mapIndexesWithConstValueId.size()) mapIndexesWithConstValueId[constValueS32] = constObject->GetId();
                        if (idOpTypeIntS32 == spvUndefinedId) idOpTypeIntS32 = constObject->GetTypeId();
                    }
                }
            }

            BytecodeChunk* bytecodeNewConsts = bytecodeUpdateController.InsertNewBytecodeChunckAt(posLastConstEnd, BytecodeUpdateController::InsertionConflictBehaviourEnum::ReturnNull);
            if (bytecodeNewConsts == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(posLastConstEnd));

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

            if (errorMessages.size() > 0) success = false;
        }
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
            TypeInstruction* resourceType = memberToMoveOut.memberType;

            TypeInstruction* resourcePointerType = GetTypePointingTo(resourceType);
            VariableInstruction* resourceVariable = GetVariablePointingTo(resourcePointerType);

            BytecodeChunk* bytecodeResourceVariable = nullptr;

            spv::Id pointerTypeId = 0;
            if (resourcePointerType == nullptr)
            {
                bytecodeResourceVariable = bytecodeUpdateController.InsertNewBytecodeChunckAt(resourceType->bytecodeEndPosition, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertLast);
                if (bytecodeResourceVariable == nullptr) { error("Failed to insert a new bytecode chunk"); break; }

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

                bytecodeResourceVariable = bytecodeUpdateController.InsertNewBytecodeChunckAt(resourcePointerType->bytecodeEndPosition, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertLast);
                if (bytecodeResourceVariable == nullptr) { error("Failed to insert a new bytecode chunk"); break; }
            }
            if (resourceVariable != nullptr) { error("a variable already exists for the resource type"); break; }

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
            }
        }
        
        //===================================================================================================================
        //===================================================================================================================
        //Create the new cbuffer structs
        for (auto itcb = listNewCbuffers.begin(); itcb != listNewCbuffers.end(); itcb++)
        {
            TypeStructMemberArray* cbuffer = *itcb;
            string& cbufferName = cbuffer->declarationName;
            string cbufferVarName = cbufferName + "_var";

            BytecodeChunk* bytecodeMergedCBufferType = bytecodeUpdateController.InsertNewBytecodeChunckAt(cbuffer->tmpTargetedBytecodePosition, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertLast);
            if (bytecodeMergedCBufferType == nullptr) { error("Failed to insert a new bytecode chunk"); break; }

            //make the cbuffer struct type
            {
                spv::Instruction cbufferType(cbuffer->structTypeId, spv::NoType, spv::OpTypeStruct);
                for (unsigned int m = 0; m < cbuffer->members.size(); ++m)
                {
                    const TypeStructMember& aStreamMember = cbuffer->members[m];
                    cbufferType.addIdOperand(aStreamMember.memberTypeId);
                }
                cbufferType.dump(bytecodeMergedCBufferType->bytecode);

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
                pointer.dump(bytecodeMergedCBufferType->bytecode);
            }
            
            //make the variable
            {
                spv::Instruction variable(cbuffer->structVariableTypeId, cbuffer->structPointerTypeId, spv::OpVariable);
                variable.addImmediateOperand(spv::StorageClass::StorageClassUniform);
                variable.dump(bytecodeMergedCBufferType->bytecode);

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

                //member attribute (if any)
                if (cbufferMember.HasAttribute())
                {
                    spv::Instruction memberNameInstr(spv::OpMemberAttribute);
                    memberNameInstr.addIdOperand(cbuffer->structTypeId);
                    memberNameInstr.addImmediateOperand(memberIndex);
                    memberNameInstr.addStringOperand(cbufferMember.attribute.c_str());
                    memberNameInstr.dump(bytecodeNewNamesAndDecocates->bytecode);
                }

                if (cbufferMember.memberOffset < 0) {
                    error("An offset has not been set for the member: " + cbufferMember.GetDeclarationNameOrSemantic());
                    break;
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

//#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
//        if (listUntouchedCbuffers.size() > 0)
//        {
//            //rename the cbuffers
//            for (auto itcb = listUntouchedCbuffers.begin(); itcb != listUntouchedCbuffers.end(); itcb++)
//            {
//                CBufferTypeData* cbuffer = *itcb;
//
//                //remove their initial name instruction (if any)
//                unsigned int posToInsert = 0;
//                if (cbuffer->posOpNameType > 0)
//                {
//                    if (posToInsert == 0) posToInsert = cbuffer->posOpNameType;
//                    int wordCount = asWordCount(cbuffer->posOpNameType);
//                    if (bytecodeUpdateController.AddPortionToRemove(cbuffer->posOpNameType, wordCount) == nullptr) { error("Failed to insert a portion to remove"); break; }
//                }
//                if (cbuffer->posOpNameVariable > 0)
//                {
//                    if (posToInsert == 0) posToInsert = cbuffer->posOpNameVariable;
//                    int wordCount = asWordCount(cbuffer->posOpNameVariable);
//                    if (bytecodeUpdateController.AddPortionToRemove(cbuffer->posOpNameVariable, wordCount) == nullptr) { error("Failed to insert a portion to remove"); break; }
//                }
//                if (posToInsert == 0) posToInsert = posFirstOpNameOrDecorate;
//                BytecodeChunk* bytecodeNameInsertion = bytecodeUpdateController.InsertNewBytecodeChunckAt(posToInsert, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertFirst);
//
//                //set the new type and variable name
//                string cbufferName = cbuffer->cbufferName;
//                string cbufferVarName = cbufferName + "_var";
//
//                spv::Instruction cbufferStructName(spv::OpName);
//                cbufferStructName.addIdOperand(cbuffer->correspondingShaderType->type->GetId());
//                cbufferStructName.addStringOperand(cbufferName.c_str());
//                cbufferStructName.dump(bytecodeNameInsertion->bytecode);
//
//                spv::Instruction variableName(spv::OpName);
//                variableName.addIdOperand(cbuffer->correspondingShaderType->variable->GetId());
//                variableName.addStringOperand(cbufferVarName.c_str());
//                variableName.dump(bytecodeNameInsertion->bytecode);
//            }
//        }
//#endif

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
                            if (constObject == nullptr) { error(string("cannot get const object for Id: ") + to_string(indexConstId)); break; }
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
                                if (bytecodeUpdateController.AddPortionToRemove(start, wordCount) == nullptr) { error("Failed to insert a portion to remove"); break; }

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

                                BytecodePortionToReplace& portionToReplace = bytecodeUpdateController.SetNewPortionToReplace(start + 1);
                                portionToReplace.SetNewValues({ typeId, resultId, newStructAccessId, memberIndexInNewCbufferConstTypeId });
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
                            bytecodeUpdateController.SetNewAtomicValueUpdate(start + 3, newId);
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
