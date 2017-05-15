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

bool SpxCompiler::GetBytecodeReflectionData(EffectReflection& effectReflection)
{
    effectReflection.Clear();

    //Find all output stages entry point
    vector<OutputStageEntryPoint> listEntryPoints;
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpEntryPoint:
                {
                    spv::ExecutionModel model = (spv::ExecutionModel)asLiteralValue(start + 1);
                    ShadingStageEnum stage = GetShadingStageForExecutionMode(model);

                    spv::Id entryFunctionId = asId(start + 2);
                    FunctionInstruction* entryFunction = GetFunctionById(entryFunctionId);
                    
                    if (stage == ShadingStageEnum::Undefined) return error("Failed to read the stage from the entry point");
                    if (entryFunction == nullptr) return error("Failed to find the entry function for id: " + to_string(entryFunctionId));

                    listEntryPoints.push_back(OutputStageEntryPoint(stage, entryFunction));
                    break;
                }

                case spv::OpName:
                case spv::OpMemberName:
                case spv::OpFunction:
                case spv::OpConstant:
                {
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }
        if (listEntryPoints.size() == 0) return error("No entry points found in the compiled bytecode");
    }

    bool success;
    success = GetAllCBufferAndResourcesBindingsReflectionDataFromBytecode(effectReflection, listEntryPoints);

    if (!success) return error("Failed to get the CBuffer reflection data from the bytecode");

    return true;
}

bool SpxCompiler::GetAllCBufferAndResourcesBindingsReflectionDataFromBytecode(EffectReflection& effectReflection, vector<OutputStageEntryPoint>& listEntryPoints)
{
    bool success = true;

    //=========================================================================================================================
    //=========================================================================================================================
    //get ALL cbuffers and resources
    // -a CBuffer is defined when a struct type has a Block decorate (those have already been parsed and set when building all maps)
    // - a resources is defined by a UniformConstant variable
    vector<CBufferTypeData*> listAllCBuffers;
    vector<CBufferTypeData*> vectorCBuffersIds;
    vector<VariableInstruction*> listAllResourceVariables;
    vector<VariableInstruction*> vectorResourcesVariableById;
    vectorCBuffersIds.resize(bound(), nullptr);
    vectorResourcesVariableById.resize(bound(), nullptr);

    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj == nullptr) continue;

        if (obj->GetKind() == ObjectInstructionTypeEnum::Type)
        {
            TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
            if (type->IsCBuffer())
            {
                CBufferTypeData* cbufferData = type->GetCBufferData();
                if (cbufferData == nullptr)
                {
                    spv::Op opCode = asOpCode(type->GetBytecodeStartPosition());
                    int wordCount = asWordCount(type->GetBytecodeStartPosition());
                    int countMembers = wordCount - 2;

#ifdef XKSLANG_DEBUG_MODE
                    if (opCode != spv::OpTypeStruct) { error(string("Invalid OpCode type for the cbuffer instruction (expected OpTypeStruct): ") + OpcodeString(opCode)); break; }
                    if (countMembers <= 0 || countMembers > compilerSettings_maxStructMembers) { error("invalid cbuffer member count"); break; }
#endif

                    //we can have a cbuffer with no cbufferData: the compiled bytecode not necessarily has OpCBufferProperties instruction
                    cbufferData = new CBufferTypeData(type->GetId(), "", type->GetName(), false, true, countMembers);
                    type->SetCBufferData(cbufferData);
                }

#ifdef XKSLANG_DEBUG_MODE
                if (cbufferData == nullptr) { error("cbuffer data missing!"); break; }
                if (cbufferData->cbufferMembersData != nullptr) { error("a cbuffer members data has already been initialized"); break; }
                if (cbufferData->cbufferCountMembers <= 0) { error("invalid cbuffer member count"); break; }
#endif

                cbufferData->cbufferTypeObject = type;
                cbufferData->cbufferPointerTypeObject = GetTypePointingTo(type);
                cbufferData->cbufferVariableTypeObject = GetVariablePointingTo(cbufferData->cbufferPointerTypeObject);
                if (cbufferData->cbufferVariableTypeObject == nullptr) { error("Failed to find the variable pointing to the cbuffer: " + to_string(type->GetId())); break; }

#ifdef XKSLANG_DEBUG_MODE
                if (type->GetId() >= vectorCBuffersIds.size()) { error("cbuffer type id is out of bound. Id: " + to_string(type->GetId())); break; }
                if (cbufferData->cbufferVariableTypeObject->GetId() >= vectorCBuffersIds.size()) { error("cbuffer variable type id is out of bound. Id: " + to_string(type->GetId())); break; }
#endif

                cbufferData->cbufferMembersData = new TypeStructMemberArray();
                cbufferData->cbufferMembersData->members.resize(cbufferData->cbufferCountMembers);
                cbufferData->isUsed = true;

                listAllCBuffers.push_back(cbufferData);
                vectorCBuffersIds[type->GetId()] = cbufferData;
                //vectorCBuffersIds[cbufferData->cbufferVariableTypeObject->GetId()] = cbufferData;
            }
        }
        else if (obj->GetKind() == ObjectInstructionTypeEnum::Variable)
        {
            VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
            spv::StorageClass storageClass = (spv::StorageClass)asLiteralValue(variable->GetBytecodeStartPosition()  + 3);
            if (storageClass == spv::StorageClass::StorageClassUniformConstant)
            {
                spv::Id variableId = variable->GetResultId();
#ifdef XKSLANG_DEBUG_MODE
                if (variableId >= vectorResourcesVariableById.size()) { error("the ressource variable id is out of bound. Id: " + to_string(variableId)); break; }
                if (vectorResourcesVariableById[variableId] != nullptr) { error("a ressource variable is defined more than once. Id: " + to_string(variableId)); break; }
#endif
                vectorResourcesVariableById[variableId] = variable;
                listAllResourceVariables.push_back(variable);
            }
        }
    }
    if (errorMessages.size() > 0) success = false;
    if (listAllCBuffers.size() == 0 && listAllResourceVariables.size() == 0) return success;

    //=========================================================================================================================
    //=========================================================================================================================
    //Retrieve information from the bytecode for all cbuffers and their members
    if (success)
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
                    const spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];
                        const string name = literalString(start + 2);
                        cbufferData->cbufferName = name;
                    }
                    break;
                }

                case spv::OpMemberAttribute:
                {
                    const spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string attribute = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].attribute = attribute;
                    }
                    break;
                }

                case spv::OpMemberName:
                {
                    spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string memberName = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
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

                    if (vectorCBuffersIds[typeId] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[typeId];

                        spv::XkslPropertyEnum cbufferType = (spv::XkslPropertyEnum)asLiteralValue(start + 2);
                        spv::XkslPropertyEnum cbufferStage = (spv::XkslPropertyEnum)asLiteralValue(start + 3);
                        unsigned int countMembers = asLiteralValue(start + 4);
                        unsigned int remainingBytes = wordCount - 5;

#ifdef XKSLANG_DEBUG_MODE
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
                    spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        const unsigned int index = asLiteralValue(start + 2);
                        const spv::Decoration dec = (spv::Decoration)asLiteralValue(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif

                        switch (dec)
                        {
                            case spv::DecorationOffset:
                            {
#ifdef XKSLANG_DEBUG_MODE
                                if (wordCount <= 4) { error("Invalid wordCount"); break; }
#endif
                                const unsigned int offsetValue = asLiteralValue(start + 4);
                                cbufferData->cbufferMembersData->members[index].memberOffset = offsetValue;
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
                        }
                    }
                    break;
                }

                case spv::OpFunction:
                case spv::OpTypeFunction:
                {
                    //all information retrieved at this point: can safely stop here
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }

        if (errorMessages.size() > 0) success = false;
    }

    //Get the reflection data for all cbuffer members
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
            const unsigned int countCbuffers = (unsigned int)listAllCBuffers.size();
            effectReflection.ConstantBuffers.resize(countCbuffers);

            for (unsigned int icb = 0; icb < countCbuffers; icb++)
            {
                CBufferTypeData* cbufferData = listAllCBuffers[icb];

                TypeInstruction* cbufferType = cbufferData->cbufferTypeObject;
                spv::Op opCode = asOpCode(cbufferType->GetBytecodeStartPosition());
                int wordCount = asWordCount(cbufferType->GetBytecodeStartPosition());
                int countMembers = wordCount - 2;

#ifdef XKSLANG_DEBUG_MODE
                if (opCode != spv::OpTypeStruct) { error(string("Invalid OpCode type for the cbuffer instruction (expected OpTypeStruct): ") + OpcodeString(opCode)); break; }
                if (countMembers != cbufferData->cbufferCountMembers) { error("Invalid cbuffer count members property"); break; }
#endif
                if (countMembers != cbufferData->cbufferMembersData->countMembers()) { error("Inconsistent number of members"); break; }

                //Set the cbuffer name
                ConstantBufferReflectionDescription& cbufferReflection = effectReflection.ConstantBuffers[icb];
                cbufferReflection.CbufferName = cbufferData->cbufferName;
                cbufferReflection.Members.resize(countMembers);

                //Get the reflection data for all cbuffer members
                unsigned int posElemStart = cbufferType->GetBytecodeStartPosition() + 2;
                for (int mIndex = 0; mIndex < countMembers; ++mIndex)
                {
                    TypeStructMember& member = cbufferData->cbufferMembersData->members[mIndex];
                    ConstantBufferMemberReflectionDescription& memberReflection = cbufferReflection.Members[mIndex];
                    memberReflection.KeyName = member.GetDeclarationNameOrSemantic();

                    //get the member type object
                    spv::Id cbufferMemberTypeId = asId(posElemStart + mIndex);
                    TypeInstruction* cbufferMemberType = GetTypeById(cbufferMemberTypeId);
                    if (cbufferMemberType == nullptr) return error("failed to find the cbuffer element type for id: " + member.GetDeclarationNameOrSemantic());

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

                    if (!GetTypeReflectionDescription(cbufferMemberType, isMatrixRowMajor, member.attribute, memberReflection.ReflectionType, startPositionOfAllMemberDecorateInstructions))
                    {
                        error("Failed to get the reflexon data for the cbuffer member: " + member.GetDeclarationNameOrSemantic());
                        break;
                    }

                    //we can retrieve the offset from the bytecode, no need to recompute it
                    if (member.memberOffset == -1) {
                        error("a cbuffer member is missing its offset decoration" + member.GetDeclarationNameOrSemantic());
                        break;
                    }

#ifdef XKSLANG_DEBUG_MODE
                    //double check: we can also compute the member offset (depending on the previous member's offset, its size, plus the new member's alignment)
                    int memberOffset = 0;
                    if (mIndex > 0)
                    {
                        ConstantBufferMemberReflectionDescription& previousMemberReflection = cbufferReflection.Members[mIndex - 1];
                        int previousMemberOffset = previousMemberReflection.Offset;
                        int previousMemberSize = previousMemberReflection.ReflectionType.Size;

                        memberOffset = previousMemberOffset + previousMemberSize;
                        int memberAlignment = memberReflection.ReflectionType.Alignment;
                        //round to pow2
                        memberOffset = (memberOffset + memberAlignment - 1) & (~(memberAlignment - 1));
                    }
                    cbufferReflection.Size = memberOffset + memberReflection.ReflectionType.Size;

                    if (memberOffset != member.memberOffset) {
                        error("Offsets between reflection data and bytecode are not similar for the member:" + member.GetDeclarationNameOrSemantic());
                        break;
                    }
#endif
                    memberReflection.Offset = member.memberOffset;
                
                }
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

    //for all the struct type: find their member names in the bytecode
    if (success)
    {
        //get the list of all struct members (or sub-members)
        vector<TypeReflectionDescription*> listStructMembers;
        unsigned int countCBuffers = (unsigned int)effectReflection.ConstantBuffers.size();
        for (unsigned int cb = 0; cb < countCBuffers; cb++)
        {
            ConstantBufferReflectionDescription& cbuffer = effectReflection.ConstantBuffers[cb];
            unsigned int countMembers = (unsigned int)cbuffer.Members.size();
            for (unsigned int m = 0; m < countMembers; m++)
            {
                cbuffer.Members[m].ReflectionType.AddAllMemberAndSubMembersOfTheGivenClass(EffectParameterReflectionClass::Struct, listStructMembers);
            }
        }

        if (listStructMembers.size() > 0)
        {
            //sort the struct members by their typeId
            vector<TypeReflectionDescription*> listStructMembersByResultId;
            listStructMembersByResultId.resize(bound(), nullptr);
            for (auto itm = listStructMembers.begin(); itm != listStructMembers.end(); itm++)
            {
                TypeReflectionDescription* structMember = *itm;
#ifdef XKSLANG_DEBUG_MODE
                if (structMember->SpvTypeId >= (unsigned int)listStructMembersByResultId.size()) { error("Id out of bound: " + to_string(structMember->SpvTypeId)); break; }
#endif
                structMember->nextTypeInList = listStructMembersByResultId[structMember->SpvTypeId];
                listStructMembersByResultId[structMember->SpvTypeId] = structMember;
            }

            //Fill the struct members name
            {
                unsigned int start = header_size;
                const unsigned int end = (unsigned int)spv.size();
                while (start < end)
                {
                    unsigned int wordCount = asWordCount(start);
                    spv::Op opCode = asOpCode(start);

                    switch (opCode)
                    {
                        case spv::OpMemberName:
                        {
                            spv::Id id = asId(start + 1);
                            int memberIndex = asLiteralValue(start + 2);
#ifdef XKSLANG_DEBUG_MODE
                            if (id >= (unsigned int)listStructMembersByResultId.size()) { error("Id out of bound: " + to_string(id)); break; }
                            if (memberIndex < 0) { error("invalid memberIndex: " + to_string(memberIndex)); break; }
#endif
                            if (listStructMembersByResultId[id] != nullptr)
                            {
                                string memberName = literalString(start + 3);
                                TypeReflectionDescription* aType = listStructMembersByResultId[id];
                                while (aType != nullptr)
                                {
#ifdef XKSLANG_DEBUG_MODE
                                    if (memberIndex >= aType->CountMembers) { error("memberIndex out of bound: " + to_string(memberIndex)); break; }
#endif
                                    aType->Members[memberIndex].Name = memberName;
                                    aType = aType->nextTypeInList;
                                }
                            }
                            break;
                        }

                        case spv::OpFunction:
                        case spv::OpTypeFunction:
                        {
                            start = end;
                            break;
                        }
                    }
                    start += wordCount;
                }
            }

        }

        if (errorMessages.size() > 0) success = false;
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //Add ResourceBindings info in EffectReflection
    // Find which cbuffer resources is used by which output stage
    if (success)
    {
        //we access the cbuffer through their variable
        for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
        {
            CBufferTypeData* cbufferData = *itcb;
            vectorCBuffersIds[cbufferData->cbufferTypeObject->GetId()] = nullptr;
            vectorCBuffersIds[cbufferData->cbufferVariableTypeObject->GetId()] = cbufferData;
        }

        for (unsigned int iStage = 0; iStage < listEntryPoints.size(); ++iStage)
        {
            FunctionInstruction* stageEntryFunction = listEntryPoints[iStage].entryFunction;
            ShadingStageEnum stage = listEntryPoints[iStage].stage;

            //reset the cbuffer flag (we insert a cbuffer once per stage)
            for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
            {
                CBufferTypeData* cbufferData = *itcb;
                cbufferData->tmpFlag = 0;
            }

            //Set all functions flag to 0 (to check a function only once)
            for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
                FunctionInstruction* aFunction = *itsf;
                aFunction->flag1 = 0;
            }

            //Find all functions being called by the stage
            vector<FunctionInstruction*> listAllFunctionsCalledByTheStage;

            vector<FunctionInstruction*> vectorFunctionsToCheck;
            vectorFunctionsToCheck.push_back(stageEntryFunction);
            stageEntryFunction->flag1 = 1;
            while (vectorFunctionsToCheck.size() > 0)
            {
                FunctionInstruction* aFunctionCalled = vectorFunctionsToCheck.back();
                vectorFunctionsToCheck.pop_back();
                listAllFunctionsCalledByTheStage.push_back(aFunctionCalled);

                //check if the function is calling any other function
                unsigned int start = aFunctionCalled->bytecodeStartPosition;
                const unsigned int end = aFunctionCalled->bytecodeEndPosition;
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
                            if (vectorCBuffersIds[structIdAccessed] != nullptr)
                            {
                                CBufferTypeData* cbufferData = vectorCBuffersIds[structIdAccessed];
                                if (cbufferData->tmpFlag == 0)
                                {
                                    cbufferData->tmpFlag = 1;
                                    effectReflection.ResourceBindings.push_back(
                                        EffectResourceBindingDescription(
                                            stage,
                                            cbufferData->cbufferName,
                                            EffectParameterReflectionClass::ConstantBuffer,
                                            EffectParameterReflectionType::ConstantBuffer
                                        ));
                                }
                            }
                            break;
                        }

                        case spv::OpFunctionCall:
                        {
                            //pile the function to go check it later
                            spv::Id functionCalledId = asId(start + 3);
                            FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                            if (anotherFunctionCalled == nullptr) {
                                error("Failed to find the function for id: " + to_string(functionCalledId));
                            }
                            else
                            {
                                if (anotherFunctionCalled->flag1 == 0) {
                                    anotherFunctionCalled->flag1 = 1;
                                    vectorFunctionsToCheck.push_back(anotherFunctionCalled); //we'll analyse the function later
                                }
                            }
                            break;
                        }
                    }

                    start += wordCount;
                }
            }

            if (errorMessages.size() > 0) { success = false; break; }
        }
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //delete allocated data
    for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
    {
        CBufferTypeData* cbufferData = *itcb;
        if (cbufferData->cbufferMembersData != nullptr)
        {
            delete cbufferData->cbufferMembersData;
            cbufferData->cbufferMembersData = nullptr;
        }
    }

    return success;
}
