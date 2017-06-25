//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "SpxCompiler.h"

using namespace std;
using namespace xkslang;

static const string globalStreamStructDefaultName = "globalStreams";
static const string globalVarOnStreamStructDefaultName = "globalStreams_var";
static const string functionInputVariableDefaultName = "__input__";
static const string functionOutputVariableDefaultName = "__output__";
static const string functionStreamVariableDefaultName = "_streams";
static const string functionParamVariableDefaultName = "param";

bool SpxCompiler::MergeStreamMembers(TypeStructMemberArray& globalListOfMergedStreamVariables)
{
    vector<TypeInstruction*> listAllShaderTypeHoldingStreamVariables;
    bool success = true;

    //Those variables will define the best positions in the bytecode to insert new stuff
    unsigned int poslastMemberDecorationNameEnd = 0;
    unsigned int poslastMemberXkslPropertiesEnd = 0;
    unsigned int poslastTypeStreamDeclarationEnd = 0;

    //===================================================================================================================
    //===================================================================================================================
    //collect the list of all stream variables
    {
        //vector<MemberDecorateData> vectorParsedMembersBuiltin;
        {
            //first pass: get all stream variables by checking OpMemberProperties, an get all semantics properties
            unsigned int start = header_size;
            const unsigned int end = (unsigned int)spv.size();
            while (start < end)
            {
                unsigned int wordCount = asWordCount(start);
                spv::Op opCode = asOpCode(start);

                switch (opCode)
                {
                //case spv::OpMemberDecorate:
                //{
                //    //store members builtin to clone them with new steam members
                //    const spv::Decoration dec = (spv::Decoration)asLiteralValue(start + 3);
                //    if (dec == spv::DecorationBuiltIn)
                //    {
                //        const spv::Id typeId = asId(start + 1);
                //        const unsigned int memberId = asLiteralValue(start + 2);
                //        const unsigned int value = asLiteralValue(start + 4);
                //        vectorParsedMembersBuiltin.push_back(MemberDecorateData(typeId, memberId, dec, value));
                //    }
                //    break;
                //}

                case spv::OpMemberProperties:
                {
                    poslastMemberXkslPropertiesEnd = start + wordCount;

                    const spv::Id typeId = asId(start + 1);
                    const unsigned int memberId = asLiteralValue(start + 2);
                    TypeInstruction* type = GetTypeById(typeId);
                    if (type == nullptr) { error(string("Cannot find the type for Id: ") + to_string(typeId)); break; }

                    int countProperties = wordCount - 3;
                    bool isStream = false, isStage = false;
                    for (int a = 0; a < countProperties; ++a)
                    {
                        int prop = asLiteralValue(start + 3 + a);
                        switch (prop)
                        {
                        case spv::XkslPropertyEnum::PropertyStream:
                            isStream = true;
                            break;
                        case spv::XkslPropertyEnum::PropertyStage:
                            isStage = true;
                            break;
                        }
                    }

                    if (isStream)
                    {
                        TypeStructMemberArray* typeStreamBufferData = nullptr;
                        if (type->streamStructData == nullptr)
                        {
                            ShaderClassData* shaderOwningTheType = type->GetShaderOwner();
                            if (shaderOwningTheType == nullptr) { error(string("A stream member does not belong to a shader: ") + type->GetName()); break; }
                            ShaderTypeData* shaderTypeData = shaderOwningTheType->GetShaderTypeDataForType(type);
                            if (shaderOwningTheType == nullptr) { error(string("Cannot find the shader type data for the stream type: ") + type->GetName()); break; }

                            typeStreamBufferData = new TypeStructMemberArray();
                            type->streamStructData = typeStreamBufferData;
                            type->connectedShaderTypeData = shaderTypeData;
                            listAllShaderTypeHoldingStreamVariables.push_back(type);
                        }
                        else typeStreamBufferData = type->streamStructData;

                        if (memberId >= typeStreamBufferData->members.size()) typeStreamBufferData->members.resize(memberId + 1);
                        typeStreamBufferData->members[memberId].structTypeId = type->GetId();
                        typeStreamBufferData->members[memberId].structMemberIndex = memberId;
                        typeStreamBufferData->members[memberId].isStream = true;
                        typeStreamBufferData->members[memberId].isStage = isStage;
                    }
                    break;
                }

                case spv::OpTypeXlslShaderClass:
                {
                    //all member property are set before: we can stop parsing the rest of the bytecode
                    start = end;
                    break;
                }
                }
                start += wordCount;
            }
            if (errorMessages.size() > 0) success = false;
        }

        if (listAllShaderTypeHoldingStreamVariables.size() == 0) {
            //no stream variables detected, can safely return there
            return true;
        }

        //second pass: get the stream variables's semantic and name
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
                case spv::OpMemberName:
                case spv::OpMemberSemanticName:
                {
                    const spv::Id typeId = asId(start + 1);
                    const unsigned int memberId = asLiteralValue(start + 2);
                    TypeInstruction* type = GetTypeById(typeId);
                    if (type == nullptr) { error(string("Cannot find the type for Id: ") + to_string(typeId)); break; }
                    if (type->streamStructData == nullptr) break;  //type has not been detected as holding stream variables in the first pass

#ifdef XKSLANG_DEBUG_MODE
                    if (memberId >= type->streamStructData->members.size()) { error("Invalid member id"); break; }
#endif

                    if (opCode == spv::OpMemberSemanticName)
                    {
                        if (start > poslastMemberXkslPropertiesEnd) poslastMemberXkslPropertiesEnd = start + wordCount;

                        string semanticName = literalString(start + 3);
                        type->streamStructData->members[memberId].semantic = semanticName;
                    }
                    else if (opCode == spv::OpMemberName)
                    {
                        poslastMemberDecorationNameEnd = start + wordCount;

                        string name = literalString(start + 3);
                        type->streamStructData->members[memberId].declarationName = name;
                    }
                    break;
                }

                case spv::OpTypeXlslShaderClass:
                {
                    //all member property are set before: we can stop parsing the rest of the bytecode
                    start = end;
                    break;
                }
                }
                start += wordCount;
            }
            if (errorMessages.size() > 0) success = false;
        }

        //finally:
        // get the stream members typeId (define when creating the stream struct, for example: OpTypeStruct 22(int) 30(fvec4))
        // also add builtins information
        if (success)
        {
            vector<spv::Id> structMembersTypeList;
            for (auto itt = listAllShaderTypeHoldingStreamVariables.begin(); itt != listAllShaderTypeHoldingStreamVariables.end(); itt++)
            {
                TypeInstruction* type = *itt;

                unsigned int start = type->bytecodeStartPosition;
                if (start > poslastTypeStreamDeclarationEnd) poslastTypeStreamDeclarationEnd = start + asWordCount(start);

                structMembersTypeList.clear();
                if (!GetStructTypeMembersTypeIdList(type, structMembersTypeList)) {
                    error(string("Failed to get the list of members type for the struct type: ") + type->GetName()); continue;
                }

                const unsigned int countMembers = (unsigned int)structMembersTypeList.size();
                if (countMembers != type->streamStructData->members.size()) { error(string("Inconsistent number of members for the type: ") + type->GetName()); continue; }
                for (unsigned int m = 0; m < countMembers; ++m)
                    type->streamStructData->members[m].memberTypeId = structMembersTypeList[m];

                //add builtins info
                //const spv::Id typeId = type->GetId();
                //for (auto itBuitIns = vectorParsedMembersBuiltin.begin(); itBuitIns != vectorParsedMembersBuiltin.end(); itBuitIns++)
                //{
                //    const MemberDecorateData& builtInData = *itBuitIns;
                //    if (builtInData.typeId == typeId)
                //    {
                //        const unsigned int memberId = builtInData.memberId;
                //        if (memberId >= type->streamStructData->members.size()) { error(string("Invalid BuiltIn member id: ") + to_string(memberId)); continue; }
                //        type->streamStructData->members[memberId].listBuiltInSemantics.push_back(builtInData.value);
                //    }
                //}
            }
            if (errorMessages.size() > 0) success = false;
        }
    }

#ifdef XKSLANG_DEBUG_MODE
    //debug: check that all streamStruct are valid
    {
        for (auto itt = listAllShaderTypeHoldingStreamVariables.begin(); itt != listAllShaderTypeHoldingStreamVariables.end(); itt++)
        {
            TypeInstruction* type = *itt;
            if (type->streamStructData == nullptr) { error("streamStructData is null"); break; }
            for (auto itm = type->streamStructData->members.begin(); itm != type->streamStructData->members.end(); itm++)
            {
                const TypeStructMember& member = *itm;
                if (member.isStream == false || member.structMemberIndex == -1) { error("a steam member is invalid"); }
                if (member.HasSemantic() && member.isStage == false) { error("a steam member defines a semantic but is not a stage"); }
                if (member.memberTypeId == spvUndefinedId || member.memberTypeId == 0) { error("a steam member has an undefined type Id"); }
            }
        }
        if (errorMessages.size() > 0) success = false;
    }
#endif

    //Id of the new global stream struct
    spv::Id streamStructTypeId = bound();
    spv::Id streamPointerStructTypeId = streamStructTypeId + 1;
    spv::Id streamVarStructTypeId = streamStructTypeId + 2;
    spv::Id newBoundId = streamStructTypeId + 3;
    vector<TypeStructMember> listOfMergedStreamVariables;

    //===================================================================================================================
    //===================================================================================================================
    //regroup all stream variables in the same struct, merge the STAGE stream variables having the same semantic (or name, if no semantic is set)
    if (success)
    {
        for (auto itt = listAllShaderTypeHoldingStreamVariables.begin(); itt != listAllShaderTypeHoldingStreamVariables.end(); itt++)
        {
            TypeInstruction* type = *itt;
            for (auto iatm = type->streamStructData->members.begin(); iatm != type->streamStructData->members.end(); iatm++)
            {
                TypeStructMember& aStreamMember = *iatm;
                int mergeWithStreamIndex = -1;

                if (aStreamMember.isStage)
                {
                    //stage variable are unique, we check if a variable with same semantic or same name already exist
                    if (aStreamMember.HasSemantic()) { //compare semantic name
                        for (unsigned int i = 0; i < listOfMergedStreamVariables.size(); ++i) {
                            if (listOfMergedStreamVariables[i].isStage && listOfMergedStreamVariables[i].semantic == aStreamMember.semantic) {
                                mergeWithStreamIndex = (int)i;
                                break;
                            }
                        }
                    }
                    else { //compare declaration name
                        for (unsigned int i = 0; i < listOfMergedStreamVariables.size(); ++i) {
                            if (listOfMergedStreamVariables[i].isStage && listOfMergedStreamVariables[i].declarationName == aStreamMember.declarationName) {
                                mergeWithStreamIndex = (int)i;
                                break;
                            }
                        }
                    }
                }

                if (mergeWithStreamIndex != -1)
                {
                    //before merging the 2 stream variables, compare their type
                    if (aStreamMember.memberTypeId != listOfMergedStreamVariables[mergeWithStreamIndex].memberTypeId)
                    {
                        //same stream but different type: return an error for now
                        error(string("2 stage stream variables: ") + aStreamMember.GetNameWithSemantic() + string(" and ")
                            + listOfMergedStreamVariables[mergeWithStreamIndex].GetNameWithSemantic() + string(", have the same semantic (or same name), but a different type"));
                        mergeWithStreamIndex = -1;
                    }
                }

                if (mergeWithStreamIndex == -1)
                {
                    //add the new stream
                    aStreamMember.newStructTypeId = streamStructTypeId;
                    aStreamMember.newStructMemberIndex = (unsigned int)listOfMergedStreamVariables.size();
                    listOfMergedStreamVariables.push_back(aStreamMember);
                }
                else
                {
                    //refer to an existing stream
                    aStreamMember.newStructTypeId = streamStructTypeId;
                    aStreamMember.newStructMemberIndex = mergeWithStreamIndex;
                }
            }
        }
        if (listOfMergedStreamVariables.size() == 0) error("No stream variables have been merged");
        if (errorMessages.size() > 0) success = false;
    }

    //===================================================================================================================
    //===================================================================================================================
    //Build the map of const value with the existing const object Id, for each index of the global stream struct
    vector<spv::Id> mapIndexesWithConstValueId;
    mapIndexesWithConstValueId.resize(listOfMergedStreamVariables.size(), spvUndefinedId);
    spv::Id idOpTypeIntS32 = spvUndefinedId;
    unsigned int posLastConstEnd = 0;
    if (success)
    {
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
                    if (constValueS32 >= 0 && constValueS32 < (int)mapIndexesWithConstValueId.size())
                        mapIndexesWithConstValueId[constValueS32] = constObject->GetId();
                    if (idOpTypeIntS32 == spvUndefinedId)
                        idOpTypeIntS32 = constObject->GetTypeId();
                }
            }
        }
        if (errorMessages.size() > 0) success = false;
    }

    //vector containing the new bytecodes to add
    vector<unsigned int> vecConstsToAdd;
    vector<unsigned int> vecTypesToAdd;
    vector<unsigned int> vecNamesAndDecorate;
    vector<unsigned int> vecXkslMemberProperties;

    //===================================================================================================================
    //===================================================================================================================
    //make the missing consts and OpTypeInt
    if (success)
    {
        if (idOpTypeIntS32 == spvUndefinedId)
        {
            spv::Instruction typeInt32(newBoundId++, spv::NoType, spv::OpTypeInt);
            typeInt32.addImmediateOperand(32);
            typeInt32.addImmediateOperand(1);
            typeInt32.dump(vecConstsToAdd);
            idOpTypeIntS32 = typeInt32.getResultId();
        }
        for (unsigned int i = 0; i < mapIndexesWithConstValueId.size(); ++i)
        {
            if (mapIndexesWithConstValueId[i] == spvUndefinedId)
            {
                spv::Instruction constant(newBoundId++, idOpTypeIntS32, spv::OpConstant);
                constant.addImmediateOperand(i);
                constant.dump(vecConstsToAdd);
                mapIndexesWithConstValueId[i] = constant.getResultId();
            }
        }
    }

    //===================================================================================================================
    //===================================================================================================================
    //we remap all accesses to previous stream variables to the new one
    if (success)
    {
        vector<TypeInstruction*> vectorVariableOfStreamBufferAccessToRemap;
        vectorVariableOfStreamBufferAccessToRemap.resize(bound(), nullptr);

        //list the variable to catch and remap (the variable of all shader stream structs we merged)
        for (auto itt = listAllShaderTypeHoldingStreamVariables.begin(); itt != listAllShaderTypeHoldingStreamVariables.end(); itt++)
        {
            TypeInstruction* streamBufferType = *itt;
            VariableInstruction* variableToRemap = streamBufferType->connectedShaderTypeData->variable;
            vectorVariableOfStreamBufferAccessToRemap[variableToRemap->GetId()] = streamBufferType;
        }

        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
            case spv::OpAccessChain:
            {
                spv::Id varId = asId(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                if (varId < 0 || varId >= vectorVariableOfStreamBufferAccessToRemap.size()) { error("varId is out of bound"); break; }
#endif
                if (vectorVariableOfStreamBufferAccessToRemap[varId] != nullptr)
                {
                    TypeInstruction* streamBufferType = vectorVariableOfStreamBufferAccessToRemap[varId];
                    spv::Id accessChainFirstIndexConstId = asId(start + 4);

                    //according to specs: index is an OpConstant when indexing into a structure
                    ConstInstruction* constObject = GetConstById(accessChainFirstIndexConstId);
                    if (constObject == nullptr) { error(string("cannot get const object for Id: ") + to_string(accessChainFirstIndexConstId)); break; }

                    int accessChainIndexValue32 = constObject->valueS32;
#ifdef XKSLANG_DEBUG_MODE
                    if (!constObject->isS32) { error("the const variable uses an invalid type. It should be intS32"); break; }
                    if (accessChainIndexValue32 < 0 || accessChainIndexValue32 >= (int)streamBufferType->streamStructData->members.size()) { error("accessChainIndexValue32 is out of bound"); break; }
#endif
                    int newAccessChainIndex = streamBufferType->streamStructData->members[accessChainIndexValue32].newStructMemberIndex;
#ifdef XKSLANG_DEBUG_MODE
                    if (newAccessChainIndex < 0 || newAccessChainIndex >= (int)mapIndexesWithConstValueId.size()) { error("newAccessChainIndex is out of bound"); break; }
#endif
                    spv::Id newAccessChainIndexConstId = mapIndexesWithConstValueId[newAccessChainIndex];
#ifdef XKSLANG_DEBUG_MODE
                    if (newAccessChainIndexConstId == spvUndefinedId) { error("invalid newAccessChainIndexConstId"); break; }
#endif
                    //we remap the accessChain variable to our global stream variable
                    setId(start + 3, streamVarStructTypeId);
                    setId(start + 4, newAccessChainIndexConstId);
                }

                break;
            }
            }
            start += wordCount;
        }

        //===================================================================================================================
        //===================================================================================================================
        //add the new global stream struct into the bytecode (even if the previous step failed, otherwise the bytecode will be corrupt)
        {
            //make the stream struct type
            spv::Instruction streamStructType(streamStructTypeId, spv::NoType, spv::OpTypeStruct);
            for (unsigned int m = 0; m < listOfMergedStreamVariables.size(); ++m)
            {
                const TypeStructMember& aStreamMember = listOfMergedStreamVariables[m];
                streamStructType.addIdOperand(aStreamMember.memberTypeId);
            }
            streamStructType.dump(vecTypesToAdd);

            spv::Instruction streamStructName(spv::OpName);
            streamStructName.addIdOperand(streamStructType.getResultId());
            streamStructName.addStringOperand(globalStreamStructDefaultName.c_str());
            streamStructName.dump(vecNamesAndDecorate);

            //make the pointer
            spv::Instruction pointer(streamPointerStructTypeId, spv::NoType, spv::OpTypePointer);
            pointer.addImmediateOperand(spv::StorageClass::StorageClassPrivate);
            pointer.addIdOperand(streamStructType.getResultId());
            pointer.dump(vecTypesToAdd);

            //make the variable
            spv::Instruction variable(streamVarStructTypeId, pointer.getResultId(), spv::OpVariable);
            variable.addImmediateOperand(spv::StorageClass::StorageClassPrivate);
            variable.dump(vecTypesToAdd);

            spv::Instruction variableName(spv::OpName);
            variableName.addIdOperand(variable.getResultId());
            variableName.addStringOperand(globalVarOnStreamStructDefaultName.c_str());
            variableName.dump(vecNamesAndDecorate);

            //Add the members info
            for (unsigned int memberIndex = 0; memberIndex < listOfMergedStreamVariables.size(); ++memberIndex)
            {
                const TypeStructMember& streamMember = listOfMergedStreamVariables[memberIndex];

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                //member name
                spv::Instruction memberName(spv::OpMemberName);
                memberName.addIdOperand(streamStructType.getResultId());
                memberName.addImmediateOperand(memberIndex);
                //if (streamMember.HasSemantic()) {
                //    memberName.addStringOperand((streamMember.semantic + (streamMember.isStage ? string("_s") : string("_")) + to_string(memberIndex)).c_str());
                //}
                //else {
                //    memberName.addStringOperand((streamMember.declarationName + (streamMember.isStage ? string("_s") : string("_")) + to_string(memberIndex)).c_str());
                //}
                memberName.addStringOperand((streamMember.declarationName + (streamMember.isStage ? string("_s") : string("_")) + to_string(memberIndex)).c_str()); //use declaration name
                memberName.dump(vecNamesAndDecorate);
#endif

                //member decorate (builtin)
                //for (unsigned int smd = 0; smd < streamMember.listBuiltInSemantics.size(); ++smd)
                //{
                //    spv::Instruction memberDecorateInstr(spv::OpMemberDecorate);
                //    memberDecorateInstr.addIdOperand(streamStructType.getResultId());
                //    memberDecorateInstr.addImmediateOperand(memberIndex);
                //    memberDecorateInstr.addImmediateOperand(spv::DecorationBuiltIn);
                //    memberDecorateInstr.addImmediateOperand(streamMember.listBuiltInSemantics[smd]);
                //    memberDecorateInstr.dump(vecNamesAndDecorate);
                //}

                //member properties
                spv::Instruction memberProperties(spv::OpMemberProperties);
                memberProperties.addIdOperand(streamStructType.getResultId());
                memberProperties.addImmediateOperand(memberIndex);
                memberProperties.addImmediateOperand(spv::PropertyStream);
                if (streamMember.isStage) memberProperties.addImmediateOperand(spv::PropertyStage);
                memberProperties.dump(vecXkslMemberProperties);

                //member semantic?
                if (streamMember.HasSemantic())
                {
                    spv::Instruction memberSemantic(spv::OpMemberSemanticName);
                    memberSemantic.addIdOperand(streamStructType.getResultId());
                    memberSemantic.addImmediateOperand(memberIndex);
                    memberSemantic.addStringOperand(streamMember.semantic.c_str());
                    memberSemantic.dump(vecXkslMemberProperties);
                }
            }

            //Add the types and dada into our bytecode
            {
                if (poslastMemberXkslPropertiesEnd > poslastTypeStreamDeclarationEnd) poslastMemberXkslPropertiesEnd = poslastTypeStreamDeclarationEnd;
                if (poslastMemberDecorationNameEnd > poslastMemberXkslPropertiesEnd) poslastMemberDecorationNameEnd = poslastMemberXkslPropertiesEnd;
                if (poslastMemberDecorationNameEnd == 0) poslastMemberDecorationNameEnd = poslastMemberXkslPropertiesEnd;
                if (posLastConstEnd == 0) posLastConstEnd = poslastTypeStreamDeclarationEnd;

                //=============================================================================================================
                // add all data in the bytecode
                //Merge types and variables (we need to merge new types AFTER all previous inserts)
                spv.insert(spv.begin() + poslastTypeStreamDeclarationEnd, vecTypesToAdd.begin(), vecTypesToAdd.end());
                //Merge consts
                spv.insert(spv.begin() + posLastConstEnd, vecConstsToAdd.begin(), vecConstsToAdd.end());
                //Merge properties
                spv.insert(spv.begin() + poslastMemberXkslPropertiesEnd, vecXkslMemberProperties.begin(), vecXkslMemberProperties.end());
                //Merge names and decorate
                spv.insert(spv.begin() + poslastMemberDecorationNameEnd, vecNamesAndDecorate.begin(), vecNamesAndDecorate.end());

                setBound(newBoundId);
            }
        }

        //Update all maps
        if (!UpdateAllMaps()) error("failed to update all maps");
        if (errorMessages.size() > 0) success = false;
    }

    //===================================================================================================================
    //===================================================================================================================
    //delete objects created for the algorithm
    for (auto itt = listAllShaderTypeHoldingStreamVariables.begin(); itt != listAllShaderTypeHoldingStreamVariables.end(); itt++)
    {
        TypeInstruction* type = *itt;
        if (type->streamStructData != nullptr) {
            delete type->streamStructData;
            type->streamStructData = nullptr;
        }
    }

    //===================================================================================================================
    //===================================================================================================================
    //Remove the unused stream buffer types from the bytecode and the data structure
    //(we could probably leave them there and have them remove later on, but it will make a cleaner bytecode)
    if (success)
    {
        vector<range_t> vecStripRanges;
        for (auto itt = listAllShaderTypeHoldingStreamVariables.begin(); itt != listAllShaderTypeHoldingStreamVariables.end(); itt++)
        {
            TypeInstruction* streamBufferType = *itt;
            ShaderTypeData* shaderTypeToRemove = streamBufferType->connectedShaderTypeData;
            if (!RemoveShaderTypeFromBytecodeAndData(shaderTypeToRemove, vecStripRanges))
            {
                error(string("Failed to remove the unused shader type: ") + streamBufferType->GetName());
                break;
            }
        }

        stripBytecode(vecStripRanges);
        if (!UpdateAllMaps()) error("failed to update all maps");
    }

    //build the stream members struct info (will be used in next algorithms)
    globalListOfMergedStreamVariables.members.clear();
    globalListOfMergedStreamVariables.structTypeId = streamStructTypeId;
    globalListOfMergedStreamVariables.structPointerTypeId = streamPointerStructTypeId;
    globalListOfMergedStreamVariables.structVariableTypeId = streamVarStructTypeId;
    for (unsigned int memberIndex = 0; memberIndex < listOfMergedStreamVariables.size(); ++memberIndex)
    {
        TypeStructMember& streamMember = listOfMergedStreamVariables[memberIndex];
        streamMember.structTypeId = streamMember.newStructTypeId;
        streamMember.structMemberIndex = streamMember.newStructMemberIndex;
        streamMember.newStructTypeId = 0;
        streamMember.newStructMemberIndex = 0;

        globalListOfMergedStreamVariables.members.push_back(streamMember);
    }
    globalListOfMergedStreamVariables.mapIndexesWithConstValueId = mapIndexesWithConstValueId;  //store for later use (so we won't have to refetch them)

    if (errorMessages.size() > 0) success = false;
    return success;
}

bool SpxCompiler::AnalyseStreamsAndCBuffersAccessesForOutputStages(vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables)
{
    //if (status != SpxRemapperStatusEnum::AAA) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_StreamsAndCBuffersAnalysed;

    spv::Id globalStreamStructVariableId = globalListOfMergedStreamVariables.structVariableTypeId;

    //Set all functions stage reserve value to undefined
    for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
        FunctionInstruction* aFunction = *itsf;
        aFunction->functionProcessingStreamForStage = ShadingStageEnum::Undefined;
    }

    //Get the IDs of all CBuffers variable access
    vector<ShaderTypeData*> listAllCBufferIds;
    listAllCBufferIds.resize(bound(), nullptr);
    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) {
        ShaderClassData* shader = *itsh;
        for (auto it = shader->shaderTypesList.begin(); it != shader->shaderTypesList.end(); it++)
        {
            ShaderTypeData* shaderType = *it;
            if (shaderType->isCBufferType())
            {
                const spv::Id cbufferVarId = shaderType->variable->GetId();  //this variable is used to access the cbuffer
#ifdef XKSLANG_DEBUG_MODE
                if (cbufferVarId >= listAllCBufferIds.size()) return error("cbufferVarId is out of bound");
                if (listAllCBufferIds[cbufferVarId] != nullptr) return error("shoudn't be possible....");
#endif
                listAllCBufferIds[cbufferVarId] = shaderType;
            }
        }
    }

    //Analyse each stage: detect which streams and cbuffer are being accessed by them
    for (unsigned int iStage = 0; iStage < outputStages.size(); iStage++)
    {
        XkslMixerOutputStage* outputStage = &(outputStages[iStage]);
        if (outputStage->entryFunction == nullptr) return error("A stage entry point function is null.");

        //===================================================================================================================
        //reset/init stage access data
        outputStage->listFunctionsCalledAndAccessingStreamMembers.clear();
        outputStage->listStreamVariablesAccessed.clear();
        outputStage->listCBuffersAccessed.clear();
        for (unsigned int m = 0; m<globalListOfMergedStreamVariables.members.size(); ++m)
            outputStage->listStreamVariablesAccessed.push_back(MemberAccessDetails());  //by default: set an empty access for each stream variable

                                                                                        //reset flag for all shader types
        for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) {
            ShaderClassData* shader = *itsh;
            for (auto it = shader->shaderTypesList.begin(); it != shader->shaderTypesList.end(); it++) {
                ShaderTypeData* shaderType = *it;
                shaderType->tmpFlag = 0;
            }
        }

        //Set all functions flag to 0 (to check a function only once)
        for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
            FunctionInstruction* aFunction = *itsf;
            aFunction->flag1 = 0;
        }

        //===================================================================================================================
        //===================================================================================================================
        //1st pass: go through the stage functions call graph and map all resultIds accessing a stream variable with the index of the stream variables being accessed
        // a function using a stream variable will be owned/reserved by the stage calling it. If another stage calls the same function it will return an error
        vector<int> vectorResultIdsAccessingAStreamVariable;
        vectorResultIdsAccessingAStreamVariable.resize(bound(), -1);
        bool anyStreamBeingAccessedByTheStage = false;

        vector<FunctionInstruction*> vectorAllFunctionsCalledByTheStage;
        vector<FunctionInstruction*> vectorFunctionsToCheck;
        vectorFunctionsToCheck.push_back(outputStage->entryFunction);
        outputStage->entryFunction->flag1 = 1;
        while (vectorFunctionsToCheck.size() > 0)
        {
            FunctionInstruction* aFunctionCalled = vectorFunctionsToCheck.back();
            vectorFunctionsToCheck.pop_back();
            vectorAllFunctionsCalledByTheStage.push_back(aFunctionCalled);
            bool isFunctionAccessingAStreamVariable = false;

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

                    //are we accessing the global stream buffer?
                    if (structIdAccessed == globalStreamStructVariableId)
                    {
                        spv::Id resultId = asId(start + 2);
                        spv::Id indexConstId = asId(start + 4);

                        ConstInstruction* constObject = GetConstById(indexConstId);
                        if (constObject == nullptr) return error(string("cannot get const object for Id: ") + to_string(indexConstId));
                        int streamMemberIndex = constObject->valueS32;
#ifdef XKSLANG_DEBUG_MODE
                        if (resultId >= vectorResultIdsAccessingAStreamVariable.size()) return error("resultId out of bound");
                        if (vectorResultIdsAccessingAStreamVariable[resultId] != -1) return error("resultId is already accessing a stream variable");
                        if (streamMemberIndex < 0 || streamMemberIndex >= (int)globalListOfMergedStreamVariables.members.size())
                            return error(string("streamMemberIndex is out of bound: ") + to_string(streamMemberIndex));
#endif
                        anyStreamBeingAccessedByTheStage = true;
                        vectorResultIdsAccessingAStreamVariable[resultId] = streamMemberIndex;
                        isFunctionAccessingAStreamVariable = true;
                    }
                    else if (listAllCBufferIds[structIdAccessed] != nullptr) //are we accessing a cbuffer
                    {
                        ShaderTypeData* cbufferAccessed = listAllCBufferIds[structIdAccessed];
                        if (cbufferAccessed->tmpFlag == 0) {
                            cbufferAccessed->tmpFlag = 1;
                            outputStage->listCBuffersAccessed.push_back(cbufferAccessed);
                        }
                    }

                    break;
                }

                case spv::OpFunctionCall:
                case spv::OpFunctionCallBaseResolved:
                case spv::OpFunctionCallThroughStaticShaderClassCall:
                {
                    //pile the function to go check it later
                    spv::Id functionCalledId = asId(start + 3);
                    FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                    if (anotherFunctionCalled->flag1 == 0) {
                        anotherFunctionCalled->flag1 = 1;
                        vectorFunctionsToCheck.push_back(anotherFunctionCalled); //we'll analyse the function later
                    }
                    break;
                }

                case spv::OpFunctionCallThroughCompositionVariable:
                case spv::OpFunctionCallBaseUnresolved:
                {
                    return error(string("An unresolved function call has been found in function: ") + aFunctionCalled->GetFullName());
                    break;
                }
                }
                start += wordCount;
            }

            if (isFunctionAccessingAStreamVariable)
            {
                if (aFunctionCalled->functionProcessingStreamForStage != ShadingStageEnum::Undefined)
                {
                    return error(GetShadingStageLabel(aFunctionCalled->functionProcessingStreamForStage) + " and " + GetShadingStageLabel(outputStage->outputStage->stage)
                        + " stages are both calling a function accessing stream members. Function name: " + aFunctionCalled->GetFullName());
                }

                aFunctionCalled->functionProcessingStreamForStage = outputStage->outputStage->stage;
                outputStage->listFunctionsCalledAndAccessingStreamMembers.push_back(aFunctionCalled);
            }
        }

        if (anyStreamBeingAccessedByTheStage)
        {
            //===================================================================================================================
            //===================================================================================================================
            // 2nd pass: go through all functions again to check all accesses to the stream variables
            // Here the order of functions called is important: if there is a function call we interrupt the current one to visit the called one first
            for (auto itf = vectorAllFunctionsCalledByTheStage.begin(); itf != vectorAllFunctionsCalledByTheStage.end(); itf++)
            {
                FunctionInstruction* aFunctionCalled = *itf;
                aFunctionCalled->currentPosInBytecode = aFunctionCalled->bytecodeStartPosition;
                aFunctionCalled->flag1 = 0;
            }

            vectorFunctionsToCheck.push_back(outputStage->entryFunction);
            outputStage->entryFunction->flag1 = 1;
            while (vectorFunctionsToCheck.size() > 0)
            {
                FunctionInstruction* aFunctionCalled = vectorFunctionsToCheck.back();
                vectorFunctionsToCheck.pop_back();

                unsigned int start = aFunctionCalled->currentPosInBytecode;
                const unsigned int end = aFunctionCalled->bytecodeEndPosition;
                while (start < end)
                {
                    unsigned int wordCount = asWordCount(start);
                    spv::Op opCode = asOpCode(start);

                    switch (opCode)
                    {
                    case spv::OpFunctionCall:
                    case spv::OpFunctionCallBaseResolved:
                    case spv::OpFunctionCallThroughStaticShaderClassCall:
                    {
                        spv::Id functionCalledId = asId(start + 3);
                        FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                        if (anotherFunctionCalled->flag1 == 0)
                        {
                            aFunctionCalled->currentPosInBytecode = start + wordCount;  //we will start again at the next function instruction
                            vectorFunctionsToCheck.push_back(aFunctionCalled);

                            //pile the function and go check it immediatly
                            anotherFunctionCalled->flag1 = 1;
                            vectorFunctionsToCheck.push_back(anotherFunctionCalled); //we'll analyse the function later

                            start = end;  //to end the loop
                        }
                        break;
                    }

                    case spv::OpStore:
                    case spv::OpLoad:
                    {
                        //is the stage storing (writing) into a stream variable?
                        spv::Id targetId = (opCode == spv::OpStore) ? asId(start + 1) : asId(start + 3);
                        if (vectorResultIdsAccessingAStreamVariable[targetId] != -1)
                        {
                            //cout << "write: " << globalListOfMergedStreamVariables.members[vectorResultIdsAccessingAStreamVariable[targetId]].declarationName << endl;
                            int streamVariableindex = vectorResultIdsAccessingAStreamVariable[targetId];
#ifdef XKSLANG_DEBUG_MODE
                            if (streamVariableindex < 0 || streamVariableindex >= (int)outputStage->listStreamVariablesAccessed.size())
                                return error(string("stream variable index is out of bound. Id: ") + to_string(streamVariableindex));
#endif
                            if (opCode == spv::OpStore) outputStage->listStreamVariablesAccessed[streamVariableindex].SetFirstAccessWrite();
                            else outputStage->listStreamVariablesAccessed[streamVariableindex].SetFirstAccessRead();
                        }
                        break;
                    }
                    }
                    start += wordCount;
                }
            }
        } //end of if (anyStreamBeingAccessedByTheStage)

    } //end of for (unsigned int iStage = 0; iStage < outputStages.size(); iStage++)

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxCompiler::ValidateStagesStreamMembersFlow(vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_StreamsAndCBuffersAnalysed) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_StreamReadyForReschuffling;

    if (outputStages.size() == 0) return error("no output stages defined");

    unsigned int countStreamMembers = (unsigned int)globalListOfMergedStreamVariables.members.size();
    if (countStreamMembers == 0) return true; //no stream variables to validate

#ifdef XKSLANG_DEBUG_MODE
    for (unsigned int ios = 0; ios < outputStages.size(); ++ios)
    {
        if (outputStages[ios].listStreamVariablesAccessed.size() != countStreamMembers)
            return error("size of the stage stream variables accessed list does not match the size of the global stream buffer");
    }
#endif

    //A stream variable is required by a stage is this variable is read-first (stream variable accessibility is set in the previous function: AnalyseStreamMembersUsageForOutputStages)
    //2 cases:
    //- If some variables are required in the Vertex stage: we assume they are defined from VB, but only if those variables have stage and declare a semantic (otherwise we return an error)
    //- For the other stages: the previous stages must output it

    //===================================================================================================================
    // For all stages but VS, set that an input stream receives an output from the previous stages
    for (int iStage = (int)outputStages.size() - 1; iStage >= 0; --iStage)
    {
        XkslMixerOutputStage& outputStage = outputStages[iStage];

        //Check special case's stream variables: some semantics are forced to be in the stage, even if not used
        if (outputStage.outputStage->stage == ShadingStageEnum::Pixel || outputStage.outputStage->stage == ShadingStageEnum::Geometry)
        {
            for (unsigned int ivs = 0; ivs < countStreamMembers; ++ivs)
            {
                if (globalListOfMergedStreamVariables.members[ivs].HasSemantic())
                {
                    if (globalListOfMergedStreamVariables.members[ivs].semantic == "SV_Position")
                    {
                        outputStage.listStreamVariablesAccessed[ivs].SetAsInput();
                    }
                    else if (globalListOfMergedStreamVariables.members[ivs].semantic == "SV_RenderTargetArrayIndex")
                    {
                        outputStage.listStreamVariablesAccessed[ivs].SetAsInput();
                    }
                }
            }
        }

        if (iStage == outputStages.size() - 1)
        {
            ///if (outputStage.outputStage->stage != ShadingStageEnum::Pixel) return error("Last output stage must be Pixel Stage");

            //final stage: it will defines the final outputs and its required inputs
            for (unsigned int ivs = 0; ivs < countStreamMembers; ++ivs)
            {
                if (outputStage.listStreamVariablesAccessed[ivs].IsReadFirstStream())
                {
                    outputStage.listStreamVariablesAccessed[ivs].SetAsInput();
                }
                else if (outputStage.listStreamVariablesAccessed[ivs].IsWriteFirstStream())
                {
                    outputStage.listStreamVariablesAccessed[ivs].SetAsOutput();
                }
            }
        }
        else
        {
            XkslMixerOutputStage& nextOutputStage = outputStages[iStage + 1];

            for (unsigned int ivs = 0; ivs < countStreamMembers; ++ivs)
            {
                if (nextOutputStage.listStreamVariablesAccessed[ivs].IsNeededAsInput())
                {
                    //we set as output the stream needed by the next stage (the stage output not needed by the next stage are ignored)
                    //if the stage does not output some streams, we require them as input as well (as passthrough)
                    if (outputStage.listStreamVariablesAccessed[ivs].IsWriteFirstStream())
                    {
                        outputStage.listStreamVariablesAccessed[ivs].SetAsOutput();
                    }
                    else
                    {
                        outputStage.listStreamVariablesAccessed[ivs].SetAsPassThrough();
                    }
                }
                else
                {
                    //if the stage needs some more inputs, we add them
                    if (outputStage.listStreamVariablesAccessed[ivs].IsReadFirstStream())
                    {
                        outputStage.listStreamVariablesAccessed[ivs].SetAsInput();
                    }
                }
            }

            if (iStage == 0)
            {
                //vertex stage: check that the input stream are set as stage and have a semantic, otherwise throw an error
                if (outputStage.outputStage->stage != ShadingStageEnum::Vertex) return error("first output stage must be a Vertex Stage");

                for (unsigned int ivs = 0; ivs < countStreamMembers; ++ivs)
                {
                    if (outputStage.listStreamVariablesAccessed[ivs].IsNeededAsInput())
                    {
                        if (!globalListOfMergedStreamVariables.members[ivs].isStage || !globalListOfMergedStreamVariables.members[ivs].HasSemantic())
                        {
                            //which stage is asking for the stream?
                            int indexStageAskingForTheInput = -1;
                            for (unsigned int i = 0; i < outputStages.size(); ++i)
                            {
                                if (outputStages[i].listStreamVariablesAccessed[ivs].IsInputOnly()) {
                                    indexStageAskingForTheInput = i;
                                    break;
                                }
                            }
                            if (indexStageAskingForTheInput == -1) return error("Internal error: indexStageAskingForTheInput == -1");  //should never happen
                            XkslMixerOutputStage& outputStageAskingTheStream = outputStages[indexStageAskingForTheInput];

                            error("The Vertex stage needs a VB input stream which is not valid (must be set as stage and have a semantic). The variable named \""
                                + globalListOfMergedStreamVariables.members[ivs].GetNameWithSemantic() + "\" is required by the " + GetShadingStageLabel(outputStageAskingTheStream.outputStage->stage) + " stage");
                        }
                    }
                }
            }
        }
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

#define RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS 1
#define PROCESS_INPUT_OUTPUT_STREAM_MEMBERS_USING_IO_VARIABLES 2
#define STREAM_MEMBERS_IO_MODE PROCESS_INPUT_OUTPUT_STREAM_MEMBERS_USING_IO_VARIABLES

bool SpxCompiler::ReshuffleStreamVariables(vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_StreamReadyForReschuffling) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_StreamReschuffled;

    //Should we init or not the new struct we insert into the function?
    bool initStreamStructsWithZeroValues = true;

    //get the global stream buffer object
    if (globalListOfMergedStreamVariables.countMembers() == 0) return true; //no stream variables to reshuffle
    TypeInstruction* globalStreamType = GetTypeById(globalListOfMergedStreamVariables.structTypeId);
    if (globalStreamType == nullptr)
        return error(string("Cannot retrieve the global stream type. Id: ") + to_string(globalListOfMergedStreamVariables.structTypeId));

    //=============================================================================================================
    //Check the list of used stream members, and init some of their data
    int countStreamMemberUsed = 0;
    for (unsigned int k = 0; k < globalListOfMergedStreamVariables.members.size(); k++) globalListOfMergedStreamVariables.members[k].isUsed = false;

    for (unsigned int iStage = 0; iStage < outputStages.size(); ++iStage)
    {
        XkslMixerOutputStage& stage = outputStages[iStage];

#ifdef XKSLANG_DEBUG_MODE
        if (stage.listStreamVariablesAccessed.size() != globalListOfMergedStreamVariables.countMembers()) return error("invalid stream variable accessed list size");
#endif

        for (unsigned int index = 0; index < stage.listStreamVariablesAccessed.size(); ++index)
        {
            const MemberAccessDetails& memberAccess = stage.listStreamVariablesAccessed[index];
            if (memberAccess.IsNeededAsInputOrOutput())
            {
                if (!globalListOfMergedStreamVariables.members[index].isUsed)
                {
                    globalListOfMergedStreamVariables.members[index].isUsed = true;
                    countStreamMemberUsed++;

                    TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                    if (streamMember.memberType == nullptr)
                    {
                        TypeInstruction* memberType = GetTypeById(streamMember.memberTypeId);
                        if (memberType == nullptr) return error("failed to find the member type for member: " + streamMember.GetDeclarationNameOrSemantic());
                        streamMember.memberType = memberType;
                    }
                }
            }
        }
    }
    if (countStreamMemberUsed == 0) return true; //no stream members being used at all, can return immediatly

    //=============================================================================================================
    //Reset some functions parameters needed for the algo
    for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
        FunctionInstruction* aFunction = *itsf;
        aFunction->functionProcessingStreamForStage = ShadingStageEnum::Undefined;
        aFunction->streamIOStructVariableResultId = 0;
        aFunction->streamIOStructConstantCompositeId = 0;
        aFunction->functionVariablesStartingPosition = 0;
    }

    //We will need to know all function call instruction to update them
    vector<FunctionCallInstructionData> listAllFunctionCallInstructions;
    if (!GetListAllFunctionCallInstructions(listAllFunctionCallInstructions))
        return error("Failed to get the list of all function call instructions");

    //=============================================================================================================
    //=============================================================================================================
    //find the best positions where to insert new stuff
    int posNewTypesInsertion = globalStreamType->bytecodeStartPosition;
    int posNamesInsertion = header_size;
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            //we'll insert the new names and decorates before the first name/decorate we find (we are certain to find at least one OpMemberProperties)
            switch (opCode)
            {
            case spv::OpName:
            case spv::OpMemberName:
            case spv::OpDeclarationName:
            case spv::OpMemberProperties:
                posNamesInsertion = start;
                start = end;
                break;
            }
            start += wordCount;
        }
    }
    
    //new bytecode chunks to insert stuff into our bytecode (all updates will be applied at once at the end)
    BytecodeUpdateController bytecodeUpdateController;
    BytecodeChunk* bytecodeNewTypes = bytecodeUpdateController.InsertNewBytecodeChunckAt(posNewTypesInsertion, BytecodeUpdateController::InsertionConflictBehaviourEnum::ReturnNull);
    BytecodeChunk* bytecodeNames = bytecodeUpdateController.InsertNewBytecodeChunckAt(posNamesInsertion, BytecodeUpdateController::InsertionConflictBehaviourEnum::ReturnNull);
    spv::Id newId = bound();

    if (bytecodeNewTypes == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(posNewTypesInsertion));
    if (bytecodeNames == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(posNamesInsertion));

    //=============================================================================================================
    //=============================================================================================================
    //when moving stream members from the global buffer to a function input/output/IO struct variable, we need to go through different pointer type (from "Private" to "Function")
    //so we create or find existing ones here
    {
        vector<int> mapStreamsMemberTypeIdToPointerFunctionTypeId;
        mapStreamsMemberTypeIdToPointerFunctionTypeId.resize(bound(), -1);

        for (unsigned int im = 0; im < globalListOfMergedStreamVariables.members.size(); ++im)
        {
            TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[im];
            streamMember.memberPointerFunctionTypeId = -1;
            mapStreamsMemberTypeIdToPointerFunctionTypeId[streamMember.memberTypeId] = 0; //0 for a stream member id with undefined pointer type
        }

        //Check if the pointer type already exists
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpTypePointer:
                {
                    spv::Id typeId = asId(start + 1);
                    spv::Id targetTypeId = asId(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                    if (targetTypeId >= mapStreamsMemberTypeIdToPointerFunctionTypeId.size()) return error("targetTypeId is out of bound");
#endif
                    if (mapStreamsMemberTypeIdToPointerFunctionTypeId[targetTypeId] == 0)
                    {
                        spv::StorageClass storageClass = (spv::StorageClass)(asLiteralValue(start + 2));
                        if (storageClass == spv::StorageClass::StorageClassFunction)
                        {
                            mapStreamsMemberTypeIdToPointerFunctionTypeId[targetTypeId] = typeId;
                        }
                    }
                    break;
                }

                case spv::OpFunction:
                    //no more type after this, can stop then
                    start = end;
                    break;                    
            }
            start += wordCount;
        }

        //assign or create the missing pointer type
        for (unsigned int im = 0; im < globalListOfMergedStreamVariables.members.size(); ++im)
        {
            TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[im];
            if (mapStreamsMemberTypeIdToPointerFunctionTypeId[streamMember.memberTypeId] == 0)
            {
                //make the struct function parameter pointer type
                spv::Instruction memberPointerType(newId++, spv::NoType, spv::OpTypePointer);
                memberPointerType.addImmediateOperand(spv::StorageClass::StorageClassFunction);
                memberPointerType.addIdOperand(streamMember.memberTypeId);
                memberPointerType.dump(bytecodeNewTypes->bytecode);
                mapStreamsMemberTypeIdToPointerFunctionTypeId[streamMember.memberTypeId] = memberPointerType.getResultId();
            }
            streamMember.memberPointerFunctionTypeId = mapStreamsMemberTypeIdToPointerFunctionTypeId[streamMember.memberTypeId];
        }
    }


    //=============================================================================================================
    //=============================================================================================================
    //Create the OpTypePointer: Output and Input types for all used stream members
#if STREAM_MEMBERS_IO_MODE == PROCESS_INPUT_OUTPUT_STREAM_MEMBERS_USING_IO_VARIABLES
    for (unsigned int iStage = 0; iStage < outputStages.size(); ++iStage)
    {
        XkslMixerOutputStage& stage = outputStages[iStage];

#ifdef XKSLANG_DEBUG_MODE
        if (stage.listStreamVariablesAccessed.size() != globalListOfMergedStreamVariables.countMembers()) return error("invalid stream variable accessed list size");
#endif

        for (unsigned int index = 0; index < stage.listStreamVariablesAccessed.size(); ++index)
        {
            if (!stage.listStreamVariablesAccessed[index].IsNeededAsInputOrOutput()) continue;
            TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

            if (stage.listStreamVariablesAccessed[index].IsNeededAsInput() && streamMember.memberTypePointerInputId == 0)
            {
                //create the TypePointer Output for the member
                spv::Instruction memberInputPointerType(newId++, spv::NoType, spv::OpTypePointer);
                memberInputPointerType.addImmediateOperand(spv::StorageClass::StorageClassInput);
                memberInputPointerType.addIdOperand(streamMember.memberTypeId);
                memberInputPointerType.dump(bytecodeNewTypes->bytecode);
                streamMember.memberTypePointerInputId = memberInputPointerType.getResultId();

                //If we have some other members with the same type, we can use the same PointerType
                for (unsigned int index2 = index + 1; index2 < globalListOfMergedStreamVariables.members.size(); index2++) {
                    TypeStructMember& anotherStreamMember = globalListOfMergedStreamVariables.members[index2];
                    if (anotherStreamMember.memberTypeId == streamMember.memberTypeId)
                        anotherStreamMember.memberTypePointerInputId = streamMember.memberTypePointerInputId;
                }
            }

            if (stage.listStreamVariablesAccessed[index].IsNeededAsOutput() && streamMember.memberTypePointerOutputId == 0)
            {
                //create the TypePointer Output for the member
                spv::Instruction memberOutputPointerType(newId++, spv::NoType, spv::OpTypePointer);
                memberOutputPointerType.addImmediateOperand(spv::StorageClass::StorageClassOutput);
                memberOutputPointerType.addIdOperand(streamMember.memberTypeId);
                memberOutputPointerType.dump(bytecodeNewTypes->bytecode);
                streamMember.memberTypePointerOutputId = memberOutputPointerType.getResultId();

                //If we have some other members with the same type, we can use the same PointerType
                for (unsigned int index2 = index + 1; index2 < globalListOfMergedStreamVariables.members.size(); index2++) {
                    TypeStructMember& anotherStreamMember = globalListOfMergedStreamVariables.members[index2];
                    if (anotherStreamMember.memberTypeId == streamMember.memberTypeId) {
                        anotherStreamMember.memberTypePointerOutputId = streamMember.memberTypePointerOutputId;
                    }
                }
            }
        }
    }
#endif

    //=============================================================================================================
    //=============================================================================================================
    //Get or create the default const object for all used stream members
    if (initStreamStructsWithZeroValues)
    {
        //get all existing const (to reuse them if we have duplicate)
        vector<ConstInstruction*> listAllConsts;
        for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it) {
            ObjectInstructionBase* obj = *it;
            if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Const) {
                ConstInstruction* constObject = dynamic_cast<ConstInstruction*>(obj);
                listAllConsts.push_back(constObject);
            }
        }

        //Get or create new const for the used stream members
        vector<spv::Instruction> listNewConstInstructionsToAdd;
        for (unsigned int index = 0; index < globalListOfMergedStreamVariables.members.size(); index++)
        {
            if (globalListOfMergedStreamVariables.members[index].isUsed)
            {
                TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                if (streamMember.memberDefaultConstantTypeId == spvUndefinedId)
                {
                    spv::Id memberDefaultConstId = GetOrCreateTypeDefaultConstValue(newId, streamMember.memberType, listAllConsts, listNewConstInstructionsToAdd);
                    if (memberDefaultConstId == 0)
                        return error("failed to create const default instructions for the member: " + streamMember.GetDeclarationNameOrSemantic());
                    streamMember.memberDefaultConstantTypeId = memberDefaultConstId;

                    //in the case of we have some other members with the same type
                    for (unsigned int index2 = index + 1; index2 < globalListOfMergedStreamVariables.members.size(); index2++)
                    {
                        TypeStructMember& anotherStreamMember = globalListOfMergedStreamVariables.members[index2];
                        if (anotherStreamMember.memberDefaultConstantTypeId == spvUndefinedId && anotherStreamMember.memberTypeId == streamMember.memberTypeId)
                        {
                            anotherStreamMember.memberDefaultConstantTypeId = streamMember.memberDefaultConstantTypeId;
                        }
                    }
                }
            }
        }

        //Did we create any new bytecode instructions?
        if (listNewConstInstructionsToAdd.size() > 0)
        {
            vector<unsigned int> bytecodeInstructionsForNewConsts;
            for (auto it = listNewConstInstructionsToAdd.begin(); it != listNewConstInstructionsToAdd.end(); it++)
            {
                const spv::Instruction& instruction = *it;
                instruction.dump(bytecodeInstructionsForNewConsts);
            }
            
            bytecodeNewTypes->bytecode.insert(bytecodeNewTypes->bytecode.end(), bytecodeInstructionsForNewConsts.begin(), bytecodeInstructionsForNewConsts.end());
        }
    }

#if STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
    //The current stage's outputs structId will be used as inputs for the next stage
    spv::Id previousStageOutputStructTypeId = spvUndefinedId;
    spv::Id previousStageOutputStructPointerTypeId = spvUndefinedId;
    vector<unsigned int> previousStageVecOutputMembersIndex;
#endif

    //=============================================================================================================
    //Create all stream struct (input, output, internal stream) for each stages, update all stream access, add all necessary data into the functions...
    for (unsigned int iStage = 0; iStage < outputStages.size(); ++iStage)
    {
        XkslMixerOutputStage& stage = outputStages[iStage];
        ShadingStageEnum shadingStageEnum = stage.outputStage->stage;
        string shadingStageLabelStr = GetShadingStageLabelShort(shadingStageEnum);

        //reinit some stuff
        for (unsigned int index = 0; index < globalListOfMergedStreamVariables.members.size(); ++index)
        {
            TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
            streamMember.memberStageOutputVariableId = 0;
            streamMember.memberStageInputVariableId = 0;
        }

        //===============================================================================
        //Find which members are needed for which IOs
        vector<unsigned int> vecStageInputMembersIndex;
        vector<unsigned int> vecStageOutputMembersIndex;
        vector<unsigned int> vecStageIOMembersIndex;
        for (unsigned int index = 0; index < stage.listStreamVariablesAccessed.size(); ++index)
        {
            bool memberAdded = false;
            if (stage.listStreamVariablesAccessed[index].IsNeededAsInput())
            {
                vecStageInputMembersIndex.push_back(index);
                vecStageIOMembersIndex.push_back(index);
                memberAdded = true;
            }
            if (stage.listStreamVariablesAccessed[index].IsNeededAsOutput())
            {
                bool canOutputStream = true;

                // if PS: pixel output is only SV_Targetx and SV_Depth, everything else are intermediate variable
                if (shadingStageEnum == ShadingStageEnum::Pixel)
                {
                    TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                    const string targetStr = "SV_Target";
                    const string depthStr = "SV_Depth";
                    if (streamMember.semantic.substr(0, targetStr.size()) != targetStr && streamMember.semantic != depthStr)
                    {
                        canOutputStream = false;
                    }
                }

                if (canOutputStream) vecStageOutputMembersIndex.push_back(index);
                if (!memberAdded) vecStageIOMembersIndex.push_back(index);
                memberAdded = true;
            }

            //in some case a stream can be accessed without being needed as input or output (so we just add the member in the IO struct)
            if (!memberAdded && stage.listStreamVariablesAccessed[index].IsBeingAccessed())
            {
                vecStageIOMembersIndex.push_back(index);
                memberAdded = true;
            }

            if (memberAdded) globalListOfMergedStreamVariables.members[index].tmpRemapToIOIndex = ((unsigned int)vecStageIOMembersIndex.size()) - 1;  //keep this info to remap the stream member to the corresponding IO
            else globalListOfMergedStreamVariables.members[index].tmpRemapToIOIndex = -1;
        }
        if (vecStageIOMembersIndex.size() == 0) continue;  //if the stage is not using any stream for input/output: no update needed for the stage

        //===============================================================================
        //Retrieve (and check validity of) the function declaration type
        FunctionInstruction* entryFunction = stage.entryFunction;

#ifdef XKSLANG_DEBUG_MODE
        if (entryFunction == nullptr) return error("Stage has no entry point function");
        spv::Op opCode = asOpCode(entryFunction->bytecodeStartPosition);
        if (opCode != spv::OpFunction) return error("Corrupted bytecode: expected OpFunction instruction");
#endif

        //make sure the stage entry function has not already been processed for a stream
        if (entryFunction->functionProcessingStreamForStage != ShadingStageEnum::Undefined) {
            return error(GetShadingStageLabel(shadingStageEnum) + " Stage entry function: " + entryFunction->GetFullName()
                + " has already been processed for streams for another stage: " + GetShadingStageLabel(entryFunction->functionProcessingStreamForStage));
        }
        entryFunction->functionProcessingStreamForStage = shadingStageEnum;

        //find and check the function declaration type
        spv::Id entryFunctionDeclarationTypeId = asId(entryFunction->bytecodeStartPosition + 4);
        spv::Id entryFunctionReturnTypeId = asId(entryFunction->bytecodeStartPosition + 1);
        TypeInstruction* entryFunctionDeclarationType = GetTypeById(entryFunctionDeclarationTypeId);
        if (entryFunctionDeclarationType == nullptr) return error("Cannot find the function declaration type for Id: " + to_string(entryFunctionDeclarationTypeId));
#ifdef XKSLANG_DEBUG_MODE
        //some sanity check
        if (asOpCode(entryFunctionDeclarationType->bytecodeStartPosition) != spv::OpTypeFunction) return error("Corrupted bytecode: function declaration type must have OpTypeFunction instruction");
        if (entryFunctionReturnTypeId != asId(entryFunctionDeclarationType->bytecodeStartPosition + 2)) return error("Corrupted bytecode: function return type must be the same as function declaration's return type");
#endif
        int functionCountParameters = asWordCount(entryFunctionDeclarationType->bytecodeStartPosition) - 3;
        if (functionCountParameters != 0) return error(GetShadingStageLabel(shadingStageEnum) + " stage's entry function cannot have any parameters");

        //confirm that the function's return type is void
        TypeInstruction* entryFunctionInitialReturnType = GetTypeById(entryFunctionReturnTypeId);
        if (entryFunctionInitialReturnType == nullptr) return error("Cannot find the function return type for Id: " + to_string(entryFunctionReturnTypeId));
        if (asOpCode(entryFunctionInitialReturnType->bytecodeStartPosition) != spv::OpTypeVoid) return error(GetShadingStageLabel(shadingStageEnum) + " stage's entry function must initially return void");

        //===============================================================================
        //input streams
        bool stageHasSomeInputs = false;

        //output streams or variables
        bool stageHasSomeOutputs = false;

        //IO stream struct
        spv::Id streamIOStructTypeId = spvUndefinedId;
        spv::Id streamIOStructConstantCompositeId = spvUndefinedId;
        spv::Id streamIOStructPointerTypeId = spvUndefinedId;

#if STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
        spv::Id streamOutputStructPointerTypeId = spvUndefinedId;
        spv::Id streamOutputStructConstantCompositeId = spvUndefinedId;
    
        spv::Id streamOutputStructTypeId = spvUndefinedId;

        spv::Id streamInputStructTypeId = spvUndefinedId;
        spv::Id streamInputStructPointerTypeId = spvUndefinedId;

        //New entry function definition
        spv::Id newEntryFunctionDeclarationTypeId = spvUndefinedId;                 //function's new declaration type
        spv::Id entryFunctionStreamInputStructParameterResultId = spvUndefinedId;   //function's new struct input parameter
        spv::Id entryFunctionStreamOutputStructVariableResultId = spvUndefinedId;
#endif

        //===============================================================================
        //create the function's input structure (for the first stage only, the next stage will use the previous stage's output structs)
        if (vecStageInputMembersIndex.size() > 0)
        {
            stageHasSomeInputs = true;

#if STREAM_MEMBERS_IO_MODE == PROCESS_INPUT_OUTPUT_STREAM_MEMBERS_USING_IO_VARIABLES

            //create the stage input variables
            for (unsigned int k = 0; k < vecStageInputMembersIndex.size(); ++k)
            {
                const unsigned int index = vecStageInputMembersIndex[k];
                TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                if (streamMember.memberTypePointerInputId == 0) return error("memberTypePointerInputId has not been set for the stream member: " + streamMember.GetDeclarationNameOrSemantic());

                //create the stage input variable
                spv::Instruction memberInputVariableInstr(newId++, streamMember.memberTypePointerInputId, spv::OpVariable);
                memberInputVariableInstr.addImmediateOperand(spv::StorageClassInput);
                memberInputVariableInstr.dump(bytecodeNewTypes->bytecode);

                //add the variable info into the stage data
                spv::Id stageVariableId = memberInputVariableInstr.getResultId();
                streamMember.memberStageInputVariableId = stageVariableId;
                XkslMixerOutputStage::OutputStageIOVariable stageVariable(stageVariableId, index, k, streamMember.semantic);
                stage.listStageInputVariableInfo.push_back(stageVariable);

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                string inputVariableName = shadingStageLabelStr + "_IN_" + streamMember.GetDeclarationNameOrSemantic();
                spv::Instruction inputVariableNameInstr(spv::OpName);
                inputVariableNameInstr.addIdOperand(memberInputVariableInstr.getResultId());
                inputVariableNameInstr.addStringOperand(inputVariableName.c_str());
                inputVariableNameInstr.dump(bytecodeNames->bytecode);
#endif
            }

            //add input variable location decorate
            for (unsigned int k = 0; k < stage.listStageInputVariableInfo.size(); ++k)
            {
                const XkslMixerOutputStage::OutputStageIOVariable& stageVariable = stage.listStageInputVariableInfo[k];

                spv::Instruction inputVariableLocation(spv::OpDecorate);
                inputVariableLocation.addIdOperand(stageVariable.spvVariableId);
                inputVariableLocation.addImmediateOperand(spv::Decoration::DecorationLocation);
                inputVariableLocation.addImmediateOperand(stageVariable.locationNum);
                inputVariableLocation.dump(bytecodeNames->bytecode);
            }

            //add the variable semantic name
            for (unsigned int k = 0; k < stage.listStageInputVariableInfo.size(); ++k)
            {
                const XkslMixerOutputStage::OutputStageIOVariable& stageVariable = stage.listStageInputVariableInfo[k];

                spv::Instruction inputVariableLocation(spv::OpSemanticName);
                inputVariableLocation.addIdOperand(stageVariable.spvVariableId);
                inputVariableLocation.addStringOperand(stageVariable.semanticName.c_str());
                inputVariableLocation.dump(bytecodeNames->bytecode);
            }

#elif STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
            //create the function's input structure (for the first stage only, the next stage will use the previous stage's output structs)
            if (iStage == 0)
            {
                //make the struct type
                spv::Instruction structType(newId++, spv::NoType, spv::OpTypeStruct);
                for (unsigned int k = 0; k < vecStageInputMembersIndex.size(); ++k)
                {
                    const unsigned int index = vecStageInputMembersIndex[k];
                    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                    structType.addIdOperand(streamMember.memberTypeId);
                }
                structType.dump(bytecodeNewTypes->bytecode);

                //make the struct function parameter pointer type
                spv::Instruction structPointerType(newId++, spv::NoType, spv::OpTypePointer);
                structPointerType.addImmediateOperand(spv::StorageClass::StorageClassFunction);
                structPointerType.addIdOperand(structType.getResultId());
                structPointerType.dump(bytecodeNewTypes->bytecode);

                streamInputStructTypeId = structType.getResultId();
                streamInputStructPointerTypeId = structPointerType.getResultId();

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                //struct name
                string stagePrefix = shadingStageLabelStr + "_IN";
                spv::Instruction structName(spv::OpName);
                structName.addIdOperand(structType.getResultId());
                structName.addStringOperand(stagePrefix.c_str());
                structName.dump(bytecodeNames->bytecode);

                //Add the members name
                for (unsigned int k = 0; k < vecStageInputMembersIndex.size(); ++k)
                {
                    const unsigned int index = vecStageInputMembersIndex[k];
                    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                    //member name
                    spv::Instruction memberName(spv::OpMemberName);
                    memberName.addIdOperand(structType.getResultId());
                    memberName.addImmediateOperand(k);
                    memberName.addStringOperand((streamMember.GetDeclarationNameOrSemantic() + "_id" + to_string(k)).c_str());
                    memberName.dump(bytecodeNames->bytecode);
                }
#endif

                //add the members builtins
                //for (unsigned int k = 0; k < vecStageInputMembersIndex.size(); ++k)
                //{
                //    const unsigned int index = vecStageInputMembersIndex[k];
                //    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                //
                //    for (unsigned int smd = 0; smd < streamMember.listBuiltInSemantics.size(); ++smd)
                //    {
                //        spv::Instruction memberDecorateInstr(spv::OpMemberDecorate);
                //        memberDecorateInstr.addIdOperand(streamInputStructTypeId);
                //        memberDecorateInstr.addImmediateOperand(k);
                //        memberDecorateInstr.addImmediateOperand(spv::DecorationBuiltIn);
                //        memberDecorateInstr.addImmediateOperand(streamMember.listBuiltInSemantics[smd]);
                //        memberDecorateInstr.dump(bytecodeNames->bytecode);
                //    }
                //}
            }
            else
            {
                //For the current stage's input structs, we can use the previous stage's output struct
                streamInputStructTypeId = previousStageOutputStructTypeId;
                streamInputStructPointerTypeId = previousStageOutputStructPointerTypeId;

                //just double check that the previous stage output stream variables match the current stage's input stream variable
                if (previousStageVecOutputMembersIndex.size() != vecStageInputMembersIndex.size())
                    return error("previous stage output streams does not match the current stage's input streams");
                for (unsigned int k = 0; k < previousStageVecOutputMembersIndex.size(); ++k)
                {
                    if (previousStageVecOutputMembersIndex[k] != vecStageInputMembersIndex[k])
                        return error("previous stage output streams does not match the current stage's input streams");
                }
            }
#else
            return error("Unknown STREAM_MEMBERS_IO_MODE");
#endif
        } //end of: if (vecStageInputMembersIndex.size() > 0)
        
        //===============================================================================
        //create the function's output data
        if (vecStageOutputMembersIndex.size() > 0)
        {
            stageHasSomeOutputs = true;

#if STREAM_MEMBERS_IO_MODE == PROCESS_INPUT_OUTPUT_STREAM_MEMBERS_USING_IO_VARIABLES
            //create the stage output variables
            for (unsigned int k = 0; k < vecStageOutputMembersIndex.size(); ++k)
            {
                const unsigned int index = vecStageOutputMembersIndex[k];
                TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                if (streamMember.memberTypePointerOutputId == 0) return error("memberTypePointerOutputId has not been set for the stream member: " + streamMember.GetDeclarationNameOrSemantic());

                //create the output variable
                spv::Instruction memberOutputVariable(newId++, streamMember.memberTypePointerOutputId, spv::OpVariable);
                memberOutputVariable.addImmediateOperand(spv::StorageClassOutput);
                memberOutputVariable.dump(bytecodeNewTypes->bytecode);

                //add the variable info into the stage data
                spv::Id stageVariableId = memberOutputVariable.getResultId();
                streamMember.memberStageOutputVariableId = stageVariableId;
                XkslMixerOutputStage::OutputStageIOVariable stageVariable(stageVariableId, index, k, streamMember.semantic);
                stage.listStageOutputVariableInfo.push_back(stageVariable);

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                string outputVariableName = shadingStageLabelStr + "_OUT_" + streamMember.GetDeclarationNameOrSemantic();
                spv::Instruction outputVariableNameInstr(spv::OpName);
                outputVariableNameInstr.addIdOperand(memberOutputVariable.getResultId());
                outputVariableNameInstr.addStringOperand(outputVariableName.c_str());
                outputVariableNameInstr.dump(bytecodeNames->bytecode);
#endif
            }

            //Add output variables location decorate
            for (unsigned int k = 0; k < stage.listStageOutputVariableInfo.size(); ++k)
            {
                const XkslMixerOutputStage::OutputStageIOVariable& stageVariable = stage.listStageOutputVariableInfo[k];

                spv::Instruction inputVariableLocation(spv::OpDecorate);
                inputVariableLocation.addIdOperand(stageVariable.spvVariableId);
                inputVariableLocation.addImmediateOperand(spv::Decoration::DecorationLocation);
                inputVariableLocation.addImmediateOperand(stageVariable.locationNum);
                inputVariableLocation.dump(bytecodeNames->bytecode);
            }

            //add the variable semantic name
            for (unsigned int k = 0; k < stage.listStageOutputVariableInfo.size(); ++k)
            {
                const XkslMixerOutputStage::OutputStageIOVariable& stageVariable = stage.listStageOutputVariableInfo[k];

                spv::Instruction inputVariableLocation(spv::OpSemanticName);
                inputVariableLocation.addIdOperand(stageVariable.spvVariableId);
                inputVariableLocation.addStringOperand(stageVariable.semanticName.c_str());
                inputVariableLocation.dump(bytecodeNames->bytecode);
            }

#elif STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS

            //output streams is returned using a struct

            //make the output struct type
            spv::Instruction structType(newId++, spv::NoType, spv::OpTypeStruct);
            for (unsigned int k = 0; k < vecStageOutputMembersIndex.size(); ++k)
            {
                const unsigned int index = vecStageOutputMembersIndex[k];
                const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                structType.addIdOperand(streamMember.memberTypeId);
            }
            structType.dump(bytecodeNewTypes->bytecode);

            //make the struct Function TypePointer
            spv::Instruction structPointerType(newId++, spv::NoType, spv::OpTypePointer);
            structPointerType.addImmediateOperand(spv::StorageClass::StorageClassFunction);
            structPointerType.addIdOperand(structType.getResultId());
            structPointerType.dump(bytecodeNewTypes->bytecode);

            streamOutputStructTypeId = structType.getResultId();
            streamOutputStructPointerTypeId = structPointerType.getResultId();

            //make the output struct constant composite type
            if (initStreamStructsWithZeroValues)
            {
                spv::Instruction structDefaultConstCompositeInstr(newId++, streamOutputStructTypeId, spv::OpConstantComposite);
                for (unsigned int k = 0; k < vecStageOutputMembersIndex.size(); ++k)
                {
                    const unsigned int index = vecStageOutputMembersIndex[k];
                    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                    if (streamMember.memberDefaultConstantTypeId == spvUndefinedId) error("The stream member type has no default const type: " + streamMember.GetDeclarationNameOrSemantic());
                    structDefaultConstCompositeInstr.addIdOperand(streamMember.memberDefaultConstantTypeId);
                }
                structDefaultConstCompositeInstr.dump(bytecodeNewTypes->bytecode);
                streamOutputStructConstantCompositeId = structDefaultConstCompositeInstr.getResultId();
            }

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //struct name
            string stagePrefix = shadingStageLabelStr + "_OUT";
            spv::Instruction structName(spv::OpName);
            structName.addIdOperand(structType.getResultId());
            structName.addStringOperand(stagePrefix.c_str());
            structName.dump(bytecodeNames->bytecode);

            //Add the members name
            for (unsigned int k = 0; k < vecStageOutputMembersIndex.size(); ++k)
            {
                const unsigned int index = vecStageOutputMembersIndex[k];
                const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                //member name
                spv::Instruction memberName(spv::OpMemberName);
                memberName.addIdOperand(structType.getResultId());
                memberName.addImmediateOperand(k);
                memberName.addStringOperand((streamMember.GetDeclarationNameOrSemantic() + "_id" + to_string(k)).c_str());
                memberName.dump(bytecodeNames->bytecode);
            }
#endif

            //add the members builtins
            //for (unsigned int k = 0; k < vecStageOutputMembersIndex.size(); ++k)
            //{
            //    const unsigned int index = vecStageOutputMembersIndex[k];
            //    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
            //
            //    for (unsigned int smd = 0; smd < streamMember.listBuiltInSemantics.size(); ++smd)
            //    {
            //        spv::Instruction memberDecorateInstr(spv::OpMemberDecorate);
            //        memberDecorateInstr.addIdOperand(streamOutputStructTypeId);
            //        memberDecorateInstr.addImmediateOperand(k);
            //        memberDecorateInstr.addImmediateOperand(spv::DecorationBuiltIn);
            //        memberDecorateInstr.addImmediateOperand(streamMember.listBuiltInSemantics[smd]);
            //        memberDecorateInstr.dump(bytecodeNames->bytecode);
            //    }
            //}
#else
            return error("Unknown STREAM_MEMBERS_IO_MODE");
#endif 
        }

        //===============================================================================
        //create the function's stream IO structure
        {
            //make the struct type
            {
                spv::Instruction structType(newId++, spv::NoType, spv::OpTypeStruct);
                for (unsigned int k = 0; k < vecStageIOMembersIndex.size(); ++k)
                {
                    const unsigned int index = vecStageIOMembersIndex[k];
                    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                    structType.addIdOperand(streamMember.memberTypeId);
                }
                structType.dump(bytecodeNewTypes->bytecode);

                //make the struct function pointer type
                spv::Instruction structPointerType(newId++, spv::NoType, spv::OpTypePointer);
                structPointerType.addImmediateOperand(spv::StorageClass::StorageClassFunction);
                structPointerType.addIdOperand(structType.getResultId());
                structPointerType.dump(bytecodeNewTypes->bytecode);

                streamIOStructTypeId = structType.getResultId();
                streamIOStructPointerTypeId = structPointerType.getResultId();

                //make the stream IO struct constant composite type
                if (initStreamStructsWithZeroValues)
                {
                    spv::Instruction structDefaultConstCompositeInstr(newId++, streamIOStructTypeId, spv::OpConstantComposite);
                    for (unsigned int k = 0; k < vecStageIOMembersIndex.size(); ++k)
                    {
                        const unsigned int index = vecStageIOMembersIndex[k];
                        const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                        if (streamMember.memberDefaultConstantTypeId == spvUndefinedId) error("The stream member type has no default const type: " + streamMember.GetDeclarationNameOrSemantic());
                        structDefaultConstCompositeInstr.addIdOperand(streamMember.memberDefaultConstantTypeId);
                    }
                    structDefaultConstCompositeInstr.dump(bytecodeNewTypes->bytecode);
                    streamIOStructConstantCompositeId = structDefaultConstCompositeInstr.getResultId();
                }
            }

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //struct name
            string stagePrefix = shadingStageLabelStr + "_STREAMS";
            spv::Instruction structName(spv::OpName);
            structName.addIdOperand(streamIOStructTypeId);
            structName.addStringOperand(stagePrefix.c_str());
            structName.dump(bytecodeNames->bytecode);

            //Add the members name
            for (unsigned int k = 0; k < vecStageIOMembersIndex.size(); ++k)
            {
                const unsigned int index = vecStageIOMembersIndex[k];
                const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                //member name
                spv::Instruction memberName(spv::OpMemberName);
                memberName.addIdOperand(streamIOStructTypeId);
                memberName.addImmediateOperand(k);
                memberName.addStringOperand((streamMember.GetDeclarationNameOrSemantic() + "_id" + to_string(k)).c_str());
                memberName.dump(bytecodeNames->bytecode);
            }
#endif
        }

#if STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
        //===============================================================================
        //If we add any output or input paramters: create the new function type (we don't update the current one, in the case of it was used by several function)
        if (streamOutputStructTypeId != spvUndefinedId || streamInputStructPointerTypeId != spvUndefinedId)
        {
            spv::Instruction functionNewTypeInstruction(newId++, spv::NoType, spv::OpTypeFunction);
            newEntryFunctionDeclarationTypeId = functionNewTypeInstruction.getResultId();

            //return type
            if (streamOutputStructTypeId != spvUndefinedId) functionNewTypeInstruction.addIdOperand(streamOutputStructTypeId);
            else functionNewTypeInstruction.addIdOperand(entryFunctionReturnTypeId);  //original return type (void)

            //input parameters
            if (streamInputStructPointerTypeId != spvUndefinedId) functionNewTypeInstruction.addIdOperand(streamInputStructPointerTypeId);  //new input struct type (if any)
            for (unsigned int pos = entryFunctionDeclarationType->bytecodeStartPosition + 3; pos < entryFunctionDeclarationType->bytecodeEndPosition; pos++)
                functionNewTypeInstruction.addIdOperand(asId(pos)); //add all previous input parameters

            functionNewTypeInstruction.dump(bytecodeNewTypes->bytecode);
        }
#endif

#if STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
        //===============================================================================
        //Add the input struct into the function parameter list
        if (streamInputStructPointerTypeId != spvUndefinedId)
        {
            unsigned int wordCount = asWordCount(entryFunction->bytecodeStartPosition);
            int entryFunctionFirstInstruction = entryFunction->bytecodeStartPosition + wordCount;
            BytecodeChunk* bytecodeFunctionParameterChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(entryFunctionFirstInstruction, BytecodeUpdateController::InsertionConflictBehaviourEnum::ReturnNull);
            if (bytecodeFunctionParameterChunk == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(entryFunctionFirstInstruction));
            spv::Instruction functionInputParameterInstr(newId++, streamInputStructPointerTypeId, spv::OpFunctionParameter);
            functionInputParameterInstr.dump(bytecodeFunctionParameterChunk->bytecode);
            entryFunctionStreamInputStructParameterResultId = functionInputParameterInstr.getResultId();

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //param name
            spv::Instruction stageInputStructName(spv::OpName);
            stageInputStructName.addIdOperand(functionInputParameterInstr.getResultId());
            stageInputStructName.addStringOperand(functionInputVariableDefaultName.c_str());
            stageInputStructName.dump(bytecodeNames->bytecode);
#endif
        }
#endif

        //===============================================================================
        // Add variables (stream, output) into the function, and initialize them
        {
            //Get the function starting and ending positions
            unsigned int functionLabelInstrPos;
            unsigned int functionReturnInstrPos;
            unsigned int functionCountReturnInstructions;
            if (!GetFunctionLabelAndReturnInstructionsPosition(entryFunction, functionLabelInstrPos, functionReturnInstrPos, functionCountReturnInstructions))
                return error("Failed to parse the function: " + entryFunction->GetFullName());
            unsigned int functionLabelInstrEndPos = functionLabelInstrPos + asWordCount(functionLabelInstrPos); //go at the end of the label instruction
            if (functionCountReturnInstructions != 1) return error("Invalid entry function number of return instruction: 1 OpReturn instruction expected.");  //For now the entry function must have one return instruction only
            if (asOpCode(functionReturnInstrPos) != spv::OpReturn) return error("Invalid entry function return instruction: OpReturn instruction expected.");  //entry function must have OpReturn (not another return type)

            //bytecode chunks to insert new types at the beginning of the function
            BytecodeChunk* functionStartNewTypeInstructionsChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionLabelInstrEndPos, BytecodeUpdateController::InsertionConflictBehaviourEnum::ReturnNull);
            if (functionStartNewTypeInstructionsChunk == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(functionLabelInstrEndPos));

            //Add the stream variable into the function variables list
            spv::Instruction functionIOStreamVariable(newId++, streamIOStructPointerTypeId, spv::OpVariable);
            functionIOStreamVariable.addImmediateOperand(spv::StorageClassFunction);
            functionIOStreamVariable.dump(functionStartNewTypeInstructionsChunk->bytecode);
            entryFunction->streamIOStructVariableResultId = functionIOStreamVariable.getResultId();
            entryFunction->streamIOStructConstantCompositeId = streamIOStructConstantCompositeId;
            entryFunction->functionVariablesStartingPosition = functionLabelInstrEndPos;  //so that we can retrieve this position later on to add more variables if need

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //stream variable name
            spv::Instruction outputVariableName(spv::OpName);
            outputVariableName.addIdOperand(functionIOStreamVariable.getResultId());
            outputVariableName.addStringOperand(functionStreamVariableDefaultName.c_str());
            outputVariableName.dump(bytecodeNames->bytecode);
#endif

            //Add, init and assign the output variables (if any)
            if (stageHasSomeOutputs)
            {
#if STREAM_MEMBERS_IO_MODE == PROCESS_INPUT_OUTPUT_STREAM_MEMBERS_USING_IO_VARIABLES
                //the entry funcion output the stream members individually

                //chunck to insert new stuff at the end of the function
                BytecodeChunk* functionFinalInstructionsChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionReturnInstrPos, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertLast);
                if (functionFinalInstructionsChunk == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(functionReturnInstrPos));

                //===============================================================================
                //copy the IO streams into the output variables
                for (unsigned int kout = 0; kout < vecStageOutputMembersIndex.size(); ++kout)
                {
                    TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[vecStageOutputMembersIndex[kout]];
                    spv::Id memberPointerFunctionTypeId = streamMember.memberPointerFunctionTypeId;
                    spv::Id memberTypeId = streamMember.memberTypeId;

                    //find the index of the corresponding member within the IO stream struct
                    int ioStreamMemberIndexConstTypeId = -1;
                    for (unsigned int kio = 0; kio < vecStageIOMembersIndex.size(); ++kio)
                    {
                        if (vecStageIOMembersIndex[kio] == vecStageOutputMembersIndex[kout]) {
                            ioStreamMemberIndexConstTypeId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[kio];
                            break;
                        }
                    }

#ifdef XKSLANG_DEBUG_MODE
                    if (streamMember.memberStageOutputVariableId == 0 || streamMember.memberStageOutputVariableId == unused) return error(string("the member stage output variable has not been created"));
                    if (memberPointerFunctionTypeId <= 0) return error(string("memberPointerFunctionTypeId has not be found or created"));
                    if (ioStreamMemberIndexConstTypeId == -1) return error(string("Failed to find the index of the corresponding member within the IO stream struct"));
#endif
                    //Access the stream IO member
                    spv::Instruction accessIOStreamInstr(newId++, memberPointerFunctionTypeId, spv::OpAccessChain);
                    accessIOStreamInstr.addIdOperand(entryFunction->streamIOStructVariableResultId);
                    accessIOStreamInstr.addImmediateOperand(ioStreamMemberIndexConstTypeId);
                    accessIOStreamInstr.dump(functionFinalInstructionsChunk->bytecode);

                    //Load the member IO value
                    spv::Instruction loadStreamMemberInstr(newId++, memberTypeId, spv::OpLoad);
                    loadStreamMemberInstr.addIdOperand(accessIOStreamInstr.getResultId());
                    loadStreamMemberInstr.dump(functionFinalInstructionsChunk->bytecode);

                    //store the member value into the output variable
                    spv::Instruction storeStreamMemberInstr(spv::OpStore);
                    storeStreamMemberInstr.addIdOperand(streamMember.memberStageOutputVariableId);
                    storeStreamMemberInstr.addIdOperand(loadStreamMemberInstr.getResultId());
                    storeStreamMemberInstr.dump(functionFinalInstructionsChunk->bytecode);
                }
#elif STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
                //the entry funcion output the stream members within a struct

                //chunck to insert new stuff at the end of the function
                BytecodeChunk* functionFinalInstructionsChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionReturnInstrPos, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertLast, 1); //1 because the OpReturn will be removed and replaced
                if (functionFinalInstructionsChunk == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(functionReturnInstrPos));

                //===============================================================================
                //Add the output struct variable into the functions
                spv::Instruction functionOutputVariable(newId++, streamOutputStructPointerTypeId, spv::OpVariable);
                functionOutputVariable.addImmediateOperand(spv::StorageClassFunction);
                functionOutputVariable.dump(functionStartNewTypeInstructionsChunk->bytecode);
                entryFunctionStreamOutputStructVariableResultId = functionOutputVariable.getResultId();

                //these fields will be used to init this struct output variable
                entryFunction->streamOutputStructVariableResultId = functionOutputVariable.getResultId();
                entryFunction->streamOutputStructConstantCompositeId = streamOutputStructConstantCompositeId;

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                //output variable name
                spv::Instruction outputVariableName(spv::OpName);
                outputVariableName.addIdOperand(functionOutputVariable.getResultId());
                outputVariableName.addStringOperand(functionOutputVariableDefaultName.c_str());
                outputVariableName.dump(bytecodeNames->bytecode);
#endif

                //update the function return type (must be equal to the function declaration type's return type)
                bytecodeUpdateController.SetNewAtomicValueUpdate(entryFunction->bytecodeStartPosition + 1, streamOutputStructTypeId);

#ifdef XKSLANG_DEBUG_MODE
                if (streamOutputStructTypeId == spvUndefinedId) return error("Invalid streamOutputStructTypeId");
                if (entryFunctionStreamOutputStructVariableResultId == spvUndefinedId) return error("Invalid entryFunctionStreamOutputStructVariableResultId");
                if (entryFunction->streamIOStructVariableResultId == 0) return error("Invalid entryFunction->streamIOStructVariableResultId");
#endif
                //===============================================================================
                //assign the output stream struct with its default const value
                if (entryFunction->streamOutputStructConstantCompositeId != 0 && entryFunction->streamOutputStructConstantCompositeId != spvUndefinedId)
                {
                    spv::Instruction setOutputStreamStructEmptyInstr(spv::OpStore);
                    setOutputStreamStructEmptyInstr.addIdOperand(entryFunction->streamOutputStructVariableResultId);
                    setOutputStreamStructEmptyInstr.addIdOperand(entryFunction->streamOutputStructConstantCompositeId);
                    setOutputStreamStructEmptyInstr.dump(functionFinalInstructionsChunk->bytecode);
                }

                //===============================================================================
                //copy the IO streams into the output variable
                for (unsigned int kout = 0; kout < vecStageOutputMembersIndex.size(); ++kout)
                {
                    TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[vecStageOutputMembersIndex[kout]];
                    spv::Id memberPointerFunctionTypeId = streamMember.memberPointerFunctionTypeId;
                    spv::Id memberTypeId = streamMember.memberTypeId;

                    //find the index of the corresponding member within the IO stream struct
                    int ioStreamMemberIndexConstTypeId = -1;
                    for (unsigned int kio = 0; kio < vecStageIOMembersIndex.size(); ++kio)
                    {
                        if (vecStageIOMembersIndex[kio] == vecStageOutputMembersIndex[kout]) {
                            ioStreamMemberIndexConstTypeId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[kio];
                            break;
                        }
                    }

#ifdef XKSLANG_DEBUG_MODE
                    if (memberPointerFunctionTypeId <= 0) return error(string("memberPointerFunctionTypeId has not be found or created"));
                    if (ioStreamMemberIndexConstTypeId == -1) return error(string("Failed to find the index of the corresponding member within the IO stream struct"));
#endif
                    //Access the stream IO member
                    spv::Instruction accessIOStreamInstr(newId++, memberPointerFunctionTypeId, spv::OpAccessChain);
                    accessIOStreamInstr.addIdOperand(entryFunction->streamIOStructVariableResultId);
                    accessIOStreamInstr.addImmediateOperand(ioStreamMemberIndexConstTypeId);
                    accessIOStreamInstr.dump(functionFinalInstructionsChunk->bytecode);

                    //Load the member IO value
                    spv::Instruction loadStreamMemberInstr(newId++, memberTypeId, spv::OpLoad);
                    loadStreamMemberInstr.addIdOperand(accessIOStreamInstr.getResultId());
                    loadStreamMemberInstr.dump(functionFinalInstructionsChunk->bytecode);

                    //Access the stream output member
                    spv::Id outputMemberIndexConstTypeId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[kout];
                    spv::Instruction accessOutputStreamInstr(newId++, memberPointerFunctionTypeId, spv::OpAccessChain);
                    accessOutputStreamInstr.addIdOperand(entryFunctionStreamOutputStructVariableResultId);
                    accessOutputStreamInstr.addImmediateOperand(outputMemberIndexConstTypeId);
                    accessOutputStreamInstr.dump(functionFinalInstructionsChunk->bytecode);

                    //store the member value into output stream
                    spv::Instruction storeStreamMemberInstr(spv::OpStore);
                    storeStreamMemberInstr.addIdOperand(accessOutputStreamInstr.getResultId());
                    storeStreamMemberInstr.addIdOperand(loadStreamMemberInstr.getResultId());
                    storeStreamMemberInstr.dump(functionFinalInstructionsChunk->bytecode);
                }

                //============================================================================================================
                //add instruction to load the output struct
                spv::Instruction loadingOutputInstruction(newId++, streamOutputStructTypeId, spv::OpLoad);
                loadingOutputInstruction.addIdOperand(functionOutputVariable.getResultId());
                loadingOutputInstruction.dump(functionFinalInstructionsChunk->bytecode);

                //OpReturn instruction to OpReturnValue 
                spv::Instruction returnValueInstruction(spv::NoResult, spv::NoType, spv::OpReturnValue);
                returnValueInstruction.addIdOperand(loadingOutputInstruction.getResultId());
                returnValueInstruction.dump(functionFinalInstructionsChunk->bytecode);
#endif
            }  //end of: Add, init and assign the output variables (if any)

            //assign the IO stream struct with its default const value (done here: after having added all function new variables)
            if (entryFunction->streamIOStructConstantCompositeId != 0 && entryFunction->streamIOStructConstantCompositeId != spvUndefinedId)
            {
                spv::Instruction setIOStreamStructEmptyInstr(spv::OpStore);
                setIOStreamStructEmptyInstr.addIdOperand(entryFunction->streamIOStructVariableResultId);
                setIOStreamStructEmptyInstr.addIdOperand(entryFunction->streamIOStructConstantCompositeId);
                setIOStreamStructEmptyInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);
            }

            //============================================================================================================
            //============================================================================================================
            //init the stage IO stream (copy all input stream into the IO stream)
            if (stageHasSomeInputs)
            {
#ifdef XKSLANG_DEBUG_MODE
                if (entryFunction->streamIOStructVariableResultId == spvUndefinedId) return error("Invalid entryFunction->streamIOStructVariableResultId");
#endif

#if STREAM_MEMBERS_IO_MODE == PROCESS_INPUT_OUTPUT_STREAM_MEMBERS_USING_IO_VARIABLES
                //////chunck to insert new stuff at the end of the function
                ////BytecodeChunk* functionFinalInstructionsChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionReturnInstrPos, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertLast);
                ////if (functionFinalInstructionsChunk == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(functionReturnInstrPos));

                //===============================================================================
                //init the IO streams from the input variables
                for (unsigned int ki = 0; ki < vecStageInputMembersIndex.size(); ++ki)
                {
                    TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[vecStageInputMembersIndex[ki]];
                    spv::Id memberPointerFunctionTypeId = streamMember.memberPointerFunctionTypeId;
                    spv::Id memberTypeId = streamMember.memberTypeId;

#ifdef XKSLANG_DEBUG_MODE
                    if (memberPointerFunctionTypeId <= 0) return error(string("memberPointerFunctionTypeId has not be found or created"));
                    if (streamMember.memberStageInputVariableId == 0 || streamMember.memberStageInputVariableId == unused) return error(string("the member stage input variable has not been created"));
#endif

                    //load the member value from the input variable
                    spv::Instruction loadStreamMemberInstr(newId++, memberTypeId, spv::OpLoad);
                    loadStreamMemberInstr.addIdOperand(streamMember.memberStageInputVariableId);
                    loadStreamMemberInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);

                    //find the index of the corresponding member within the IO stream struct
                    int ioStreamMemberIndexConstTypeId = -1;
                    for (unsigned int kio = 0; kio < vecStageIOMembersIndex.size(); ++kio)
                    {
                        if (vecStageIOMembersIndex[kio] == vecStageInputMembersIndex[ki]) {
                            ioStreamMemberIndexConstTypeId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[kio];
                            break;
                        }
                    }
#ifdef XKSLANG_DEBUG_MODE
                    if (ioStreamMemberIndexConstTypeId == -1) return error(string("Failed to find the index of the corresponding member within the IO stream struct"));
#endif
                    //Access the stream IO member
                    spv::Instruction accessIOStreamInstr(newId++, memberPointerFunctionTypeId, spv::OpAccessChain);
                    accessIOStreamInstr.addIdOperand(entryFunction->streamIOStructVariableResultId);
                    accessIOStreamInstr.addImmediateOperand(ioStreamMemberIndexConstTypeId);
                    accessIOStreamInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);

                    //store the member value into IO stream
                    spv::Instruction storeStreamMemberInstr(spv::OpStore);
                    storeStreamMemberInstr.addIdOperand(accessIOStreamInstr.getResultId());
                    storeStreamMemberInstr.addIdOperand(loadStreamMemberInstr.getResultId());
                    storeStreamMemberInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);
                }

#elif STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
#ifdef XKSLANG_DEBUG_MODE
                if (entryFunctionStreamInputStructParameterResultId == spvUndefinedId) return error("Invalid entryFunctionStreamInputStructParameterResultId");
#endif
                for (unsigned int ki = 0; ki < vecStageInputMembersIndex.size(); ++ki)
                {
                    TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[vecStageInputMembersIndex[ki]];
                    spv::Id memberPointerFunctionTypeId = streamMember.memberPointerFunctionTypeId;
                    spv::Id memberTypeId = streamMember.memberTypeId;

#ifdef XKSLANG_DEBUG_MODE
                    if (memberPointerFunctionTypeId <= 0) return error(string("memberPointerFunctionTypeId has not be found or created"));
#endif
                    //Access the stream input member
                    spv::Id inputMemberIndexConstTypeId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[ki];
                    spv::Instruction accessInputStreamInstr(newId++, memberPointerFunctionTypeId, spv::OpAccessChain);
                    accessInputStreamInstr.addIdOperand(entryFunctionStreamInputStructParameterResultId);
                    accessInputStreamInstr.addImmediateOperand(inputMemberIndexConstTypeId);
                    accessInputStreamInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);
                    
                    //Load the member input value
                    spv::Instruction loadStreamMemberInstr(newId++, memberTypeId, spv::OpLoad);
                    loadStreamMemberInstr.addIdOperand(accessInputStreamInstr.getResultId());
                    loadStreamMemberInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);

                    //find the index of the corresponding member within the IO stream struct
                    int ioStreamMemberIndexConstTypeId = -1;
                    for (unsigned int kio = 0; kio < vecStageIOMembersIndex.size(); ++kio)
                    {
                        if (vecStageIOMembersIndex[kio] == vecStageInputMembersIndex[ki]) {
                            ioStreamMemberIndexConstTypeId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[kio];
                            break;
                        }
                    }
#ifdef XKSLANG_DEBUG_MODE
                    if (ioStreamMemberIndexConstTypeId == -1) return error(string("Failed to find the index of the corresponding member within the IO stream struct"));
#endif
                    //Access the stream IO member
                    spv::Instruction accessIOStreamInstr(newId++, memberPointerFunctionTypeId, spv::OpAccessChain);
                    accessIOStreamInstr.addIdOperand(entryFunction->streamIOStructVariableResultId);
                    accessIOStreamInstr.addImmediateOperand(ioStreamMemberIndexConstTypeId);
                    accessIOStreamInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);

                    //store the member value into IO stream
                    spv::Instruction storeStreamMemberInstr(spv::OpStore);
                    storeStreamMemberInstr.addIdOperand(accessIOStreamInstr.getResultId());
                    storeStreamMemberInstr.addIdOperand(loadStreamMemberInstr.getResultId());
                    storeStreamMemberInstr.dump(functionStartNewTypeInstructionsChunk->bytecode);
                }
#endif //end of: #elif STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
            } //end of: if (stageHasSomeInputs)
        }

#if STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
        //===============================================================================
        //update the function declaration type to refer to new one (if any)
        if (newEntryFunctionDeclarationTypeId != spvUndefinedId)
        {
            bytecodeUpdateController.SetNewAtomicValueUpdate(entryFunction->bytecodeStartPosition + 4, newEntryFunctionDeclarationTypeId);
        }
#endif

#if STREAM_MEMBERS_IO_MODE == RESCHUFFLE_INPUT_OUTPUT_STREAM_MEMBERS_INTO_STRUCTS
        //===============================================================================
        //Current stage's output data will be used by the next stage input data
        previousStageOutputStructTypeId = streamOutputStructTypeId;
        previousStageOutputStructPointerTypeId = streamOutputStructPointerTypeId;
        previousStageVecOutputMembersIndex = vecStageOutputMembersIndex;
#endif

        vector<FunctionInstruction*> listAllFunctionsUpdatedWithIOStreams; //list all functions where we'll need to replace access from global stream to the new variable/parameter ioStream
        listAllFunctionsUpdatedWithIOStreams.push_back(entryFunction);

        //=================================================================================================================
        //=================================================================================================================
        //Add the inout streams into every other functions called by the current stage (if they need to access it)
        {
            vector<FunctionInstruction*> listFunctionToUpdateWithIOStreams;
            for (auto it = stage.listFunctionsCalledAndAccessingStreamMembers.begin(); it != stage.listFunctionsCalledAndAccessingStreamMembers.end(); it++)
            {
                FunctionInstruction* functionAccessingStreams = *it;
                if (functionAccessingStreams == entryFunction) continue; //don't put the entry function (already treated)
                listFunctionToUpdateWithIOStreams.push_back(functionAccessingStreams);
            }

            while (listFunctionToUpdateWithIOStreams.size() > 0)
            {
                FunctionInstruction* functionAccessingStreams = listFunctionToUpdateWithIOStreams.back();
                listFunctionToUpdateWithIOStreams.pop_back();

                //Some Checks
                if (functionAccessingStreams->functionProcessingStreamForStage == shadingStageEnum) continue;  //the function was already treated for the current stage
                if (functionAccessingStreams->functionProcessingStreamForStage != ShadingStageEnum::Undefined)
                {
                    return error(GetShadingStageLabel(functionAccessingStreams->functionProcessingStreamForStage) + " and " + GetShadingStageLabel(shadingStageEnum)
                        + " stages are both calling a function accessing stream members. Function name: " + functionAccessingStreams->GetFullName());
                }
                functionAccessingStreams->functionProcessingStreamForStage = shadingStageEnum; //Function is now reserved for the current stage (ie a function from another stage cannot call this one)
                listAllFunctionsUpdatedWithIOStreams.push_back(functionAccessingStreams);

                //Update the function by adding the stream inout parameter
                {
                    //===========================================================================================
                    //get the original function declaration type
                    spv::Id functionDeclarationTypeId = asId(functionAccessingStreams->bytecodeStartPosition + 4);
                    spv::Id functionReturnTypeId = asId(functionAccessingStreams->bytecodeStartPosition + 1);
                    TypeInstruction* functionDeclarationType = GetTypeById(functionDeclarationTypeId);
                    if (functionDeclarationType == nullptr) return error("Cannot find the function declaration type for Id: " + to_string(functionDeclarationTypeId));

#ifdef XKSLANG_DEBUG_MODE
                    //some sanity check
                    if (asOpCode(functionDeclarationType->bytecodeStartPosition) != spv::OpTypeFunction) return error("Corrupted bytecode: function declaration type must have OpTypeFunction instruction");
#endif
                    //===========================================================================================
                    //create the new function declaration type
                    spv::Instruction functionNewTypeInstruction(newId++, spv::NoType, spv::OpTypeFunction);
                    spv::Id functionNewDeclarationTypeId = functionNewTypeInstruction.getResultId();
                    //return type: same as the original one
                    functionNewTypeInstruction.addIdOperand(functionReturnTypeId);  
                    //input parameters: add the inout stream, follow by the previous parameters
                    functionNewTypeInstruction.addIdOperand(streamIOStructPointerTypeId);
                    for (unsigned int pos = functionDeclarationType->bytecodeStartPosition + 3; pos < functionDeclarationType->bytecodeEndPosition; pos++)
                        functionNewTypeInstruction.addIdOperand(asId(pos)); //add all previous input parameters
                    functionNewTypeInstruction.dump(bytecodeNewTypes->bytecode);

                    //===========================================================================================
                    //update the function's declaration type with the new one
                    bytecodeUpdateController.SetNewAtomicValueUpdate(functionAccessingStreams->bytecodeStartPosition + 4, functionNewDeclarationTypeId);

                    //===========================================================================================
                    //Get the function OpLabel position
                    unsigned int functionLabelInstrPos;
                    if (!GetFunctionLabelInstructionPosition(functionAccessingStreams, functionLabelInstrPos)) return error("Failed to parse the function: " + functionAccessingStreams->GetFullName());
                    unsigned int functionLabelInstrEndPos = functionLabelInstrPos + asWordCount(functionLabelInstrPos); //go at the end of the label instruction

                    //===========================================================================================
                    //Add the new io stream parameter in the function bytecode
                    unsigned int wordCount = asWordCount(functionAccessingStreams->bytecodeStartPosition);
                    int functionFirstInstruction = functionAccessingStreams->bytecodeStartPosition + wordCount;
                    BytecodeChunk* bytecodeFunctionParameterChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionFirstInstruction, BytecodeUpdateController::InsertionConflictBehaviourEnum::ReturnNull);
                    if (bytecodeFunctionParameterChunk == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(functionFirstInstruction));
                    spv::Instruction functionStreamParameterInstr(newId++, streamIOStructPointerTypeId, spv::OpFunctionParameter);
                    functionStreamParameterInstr.dump(bytecodeFunctionParameterChunk->bytecode);
                    functionAccessingStreams->streamIOStructVariableResultId = functionStreamParameterInstr.getResultId();
                    functionAccessingStreams->functionVariablesStartingPosition = functionLabelInstrEndPos; //so that we can retrieve this position later on to add more variables if need

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                    //param name
                    spv::Instruction stageInputStructName(spv::OpName);
                    stageInputStructName.addIdOperand(functionStreamParameterInstr.getResultId());
                    stageInputStructName.addStringOperand(functionStreamVariableDefaultName.c_str());
                    stageInputStructName.dump(bytecodeNames->bytecode);
#endif
                }

                //any functions calling this function will need to be updated as well
                for (auto itf = listAllFunctionCallInstructions.begin(); itf != listAllFunctionCallInstructions.end(); itf++)
                {
                    const FunctionCallInstructionData& aFunctionCall = *itf;

                    if (aFunctionCall.functionCalled == functionAccessingStreams)
                    {
                        if (aFunctionCall.functionCalling->functionProcessingStreamForStage == ShadingStageEnum::Undefined)
                        {
                            listFunctionToUpdateWithIOStreams.push_back(aFunctionCall.functionCalling);
                        }
                    }
                }
            } //end while (listFunctionToUpdateWithIOStreams.size() > 0)
        }  //end of add the inout streams into every other functions called for the stage (if they need to access it)

        //===============================================================================
        //Update every functions to replace access to global stream struct with new ioStream variables / parameter
        while (listAllFunctionsUpdatedWithIOStreams.size() > 0)
        {
            FunctionInstruction* aFunctionAccessingStream = listAllFunctionsUpdatedWithIOStreams.back();
            listAllFunctionsUpdatedWithIOStreams.pop_back();

            unsigned int start = aFunctionAccessingStream->bytecodeStartPosition;
            const unsigned int end = aFunctionAccessingStream->bytecodeEndPosition;
            while (start < end)
            {
                unsigned int wordCount = asWordCount(start);
                spv::Op opCode = asOpCode(start);

                switch (opCode)
                {
                    case spv::OpAccessChain:
                    {
                        spv::Id structIdAccessed = asId(start + 3);

                        //are we accessing the global stream buffer?
                        if (structIdAccessed == globalListOfMergedStreamVariables.structVariableTypeId)
                        {
                            spv::Id resultId = asId(start + 2);
                            spv::Id indexConstId= asId(start + 4);

                            ConstInstruction* constObject = GetConstById(indexConstId);
                            if (constObject == nullptr) return error(string("cannot get const object for Id: ") + to_string(indexConstId));
                            int globalStreamMemberIndex = constObject->valueS32;  //index of the member in the original global stream struct
#ifdef XKSLANG_DEBUG_MODE
                            if (!constObject->isS32) return error("const object is not a valid S32");
                            if (globalStreamMemberIndex < 0 || globalStreamMemberIndex >= (int)globalListOfMergedStreamVariables.members.size())
                                return error(string("globalStreamMemberIndex is out of bound: ") + to_string(globalStreamMemberIndex));
                            if (globalListOfMergedStreamVariables.members[globalStreamMemberIndex].tmpRemapToIOIndex == -1)
                                return error("The global stream member has not been selected as IO stage stream");
#endif
                            int ioStreamMemberIndex = globalListOfMergedStreamVariables.members[globalStreamMemberIndex].tmpRemapToIOIndex; //new index for the members in the IO stream
#ifdef XKSLANG_DEBUG_MODE
                            if (ioStreamMemberIndex < 0 || ioStreamMemberIndex >= (int)vecStageIOMembersIndex.size())
                                return error(string("ioStreamMemberIndex is out of bound: ") + to_string(ioStreamMemberIndex));
#endif
                            spv::Id ioMemberConstId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[ioStreamMemberIndex]; //get the const type id for new index

                            //We change the OpAccessChain's pointertype (typeId) from private to function
                            spv::Id memberPointerFunctionTypeId = globalListOfMergedStreamVariables.members[globalStreamMemberIndex].memberPointerFunctionTypeId;
#ifdef XKSLANG_DEBUG_MODE
                            if (memberPointerFunctionTypeId <= 0) return error(string("memberPointerFunctionTypeId has not be found or created"));
#endif
                            BytecodePortionToReplace& portionToReplace = bytecodeUpdateController.SetNewPortionToReplace(start + 1);
                            portionToReplace.SetNewValues( {memberPointerFunctionTypeId, resultId, aFunctionAccessingStream->streamIOStructVariableResultId, ioMemberConstId} );
                        }
                        break;
                    }
                }
                start += wordCount;
            }
        }  //end of: Update every functions to replace access to global stream struct with new ioStream variables / parameter

        //=================================================================================================================
        //check all function call: if we call a function previously updated with the new IO stream parameter: we add the new io stream parameter into the function call
        for (auto itf = listAllFunctionCallInstructions.begin(); itf != listAllFunctionCallInstructions.end(); itf++)
        {
            const FunctionCallInstructionData& aFunctionCall = *itf;
            if (aFunctionCall.functionCalled->functionProcessingStreamForStage == shadingStageEnum)
            {
                spv::Id functionCallStreamParameterIdToAdd = aFunctionCall.functionCalling->streamIOStructVariableResultId;
                unsigned int functionCallInstructionWordCount = asWordCount(aFunctionCall.bytecodePos);
                unsigned int functionCallingVariablesStartingPosition = aFunctionCall.functionCalling->functionVariablesStartingPosition;

                //==================================================================
                //do some checks
                if (aFunctionCall.functionCalled == entryFunction) return error("The stage entry function cannot be called");
                if (aFunctionCall.functionCalling->functionProcessingStreamForStage != shadingStageEnum)
                    return error("Cannot call a function having processed stream, if the calling function has not processed stream for the same stage");
#ifdef XKSLANG_DEBUG_MODE
                if (functionCallingVariablesStartingPosition == 0)
                    return error("The function's variable starting position has not been set for: " + aFunctionCall.functionCalling->GetFullName());
                if (functionCallStreamParameterIdToAdd == 0)
                    return error("A function calling another function upgraded with a stream IO paramers, has no IO stream to pass. Function: " + aFunctionCall.functionCalling->GetFullName());
                if (functionCallInstructionWordCount < 4) return error("Corrupted code: Invalid OpFunctionCall instruction.");
                if (functionCallInstructionWordCount > 4) {
                    //function already have some parameters: make sure the first one is not the parameter we want to add
                    spv::Id firstParameterId = asId(aFunctionCall.bytecodePos + 4);
                    if (firstParameterId == functionCallStreamParameterIdToAdd) return error("The function call is already passing the io stream parameter");
                }
#endif

                //==================================================================
                //Add the stream parameter into the function call  (Actually not working in some specific cases)
                unsigned int functionCallFirstParameterPosition = aFunctionCall.bytecodePos + 4;
                BytecodeChunk* bytecodeStreamFunctionCall = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionCallFirstParameterPosition, BytecodeUpdateController::InsertionConflictBehaviourEnum::ReturnNull);
                if (bytecodeStreamFunctionCall == nullptr) return error("Failed to insert a new bytecode chunk. position is already used: " + to_string(functionCallFirstParameterPosition));
                bytecodeStreamFunctionCall->bytecode.push_back(functionCallStreamParameterIdToAdd);
                //Update the instruction wordCount
                unsigned int newInstructionOpWord = combineOpAndWordCount(aFunctionCall.opCode, functionCallInstructionWordCount + 1);
                bytecodeUpdateController.SetNewAtomicValueUpdate(aFunctionCall.bytecodePos, newInstructionOpWord); //update instruction opWord

                /*
                ///Instead of directly calling the function with the inout stream variable, we create an intermediary variable (to follow to SPIRV semantics)
                BytecodeChunk* bytecodeChunkFunctionVariable = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionCallingVariablesStartingPosition, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertFirst);
                if (bytecodeChunkFunctionVariable == nullptr) return error("Failed to insert a new bytecode chunk");

                //Add the param variable into the function variables list
                std::vector<std::uint32_t> paramVariableInstruction;
                spv::Instruction functionStreamCopyParamVariable(newId++, streamIOStructPointerTypeId, spv::OpVariable);
                functionStreamCopyParamVariable.addImmediateOperand(spv::StorageClassFunction);
                functionStreamCopyParamVariable.dump(paramVariableInstruction);
                spv::Id copyParamVariableId = functionStreamCopyParamVariable.getResultId();

                //Insert the instruction at the beginning of the chunk (to make sure we're adding the variable directly after OpLabel instruction)
                bytecodeChunkFunctionVariable->bytecode.insert(bytecodeChunkFunctionVariable->bytecode.begin(), paramVariableInstruction.begin(), paramVariableInstruction.end());

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                //param variable name
                spv::Instruction outputVariableName(spv::OpName);
                outputVariableName.addIdOperand(copyParamVariableId);
                outputVariableName.addStringOperand(functionParamVariableDefaultName.c_str());
                outputVariableName.dump(bytecodeNames->bytecode);
#endif
                //we'll overlap the full functionCall instruction, to replace "OpFunctionCall X Y" by "OpLoad stream; OpStore param; OpFunctionCall X param Y; OpLoad param; OpStore stream"
                BytecodeChunk* bytecodeChunkStreamFunctionCall = bytecodeUpdateController.InsertNewBytecodeChunckAt(aFunctionCall.bytecodePos, BytecodeUpdateController::InsertionConflictBehaviourEnum::InsertLast, functionCallInstructionWordCount);
                if (bytecodeChunkFunctionVariable == nullptr) return error("Failed to insert a new bytecode chunk");

                std::vector<std::uint32_t>& bytecodeStreamFunctionCall = bytecodeChunkStreamFunctionCall->bytecode;

                //OpLoad (load stream)
                spv::Instruction loadIOStreamInstr(newId++, streamIOStructTypeId, spv::OpLoad);
                loadIOStreamInstr.addIdOperand(functionCallStreamParameterIdToAdd);
                loadIOStreamInstr.dump(bytecodeStreamFunctionCall);

                //OpStore (copy stream to param)
                spv::Instruction storeIoStreamToParamInstr(spv::OpStore);
                storeIoStreamToParamInstr.addIdOperand(copyParamVariableId);
                storeIoStreamToParamInstr.addIdOperand(loadIOStreamInstr.getResultId());
                storeIoStreamToParamInstr.dump(bytecodeStreamFunctionCall);

                //OpFunctionCall
                unsigned int newInstructionOpWord = combineOpAndWordCount(aFunctionCall.opCode, functionCallInstructionWordCount + 1);
                bytecodeStreamFunctionCall.push_back(newInstructionOpWord);
                bytecodeStreamFunctionCall.insert(bytecodeStreamFunctionCall.end(), spv.begin() + (aFunctionCall.bytecodePos + 1), spv.begin() + (aFunctionCall.bytecodePos + 4));
                bytecodeStreamFunctionCall.push_back(copyParamVariableId);
                if (functionCallInstructionWordCount > 4)
                    bytecodeStreamFunctionCall.insert(bytecodeStreamFunctionCall.end(), spv.begin() + (aFunctionCall.bytecodePos + 4), spv.begin() + (aFunctionCall.bytecodePos + functionCallInstructionWordCount));

                //OpLoad (load param)
                spv::Instruction loadParamVariableInstr(newId++, streamIOStructTypeId, spv::OpLoad);
                loadParamVariableInstr.addIdOperand(copyParamVariableId);
                loadParamVariableInstr.dump(bytecodeStreamFunctionCall);

                //OpStore (copy back param to stream)
                spv::Instruction storeParamToIoStream(spv::OpStore);
                storeParamToIoStream.addIdOperand(functionCallStreamParameterIdToAdd);
                storeParamToIoStream.addIdOperand(loadParamVariableInstr.getResultId());
                storeParamToIoStream.dump(bytecodeStreamFunctionCall);*/
            }
        }  //end of check all function call: if we call a function previously updated with the new IO stream parameter: we add the new io stream parameter into the function call

    }  //end loop for (unsigned int iStage = 0; iStage < outputStages.size(); ++iStage)
    if (errorMessages.size() > 0) return false;

    //=============================================================================================================
    //=============================================================================================================
    //Update the bytecode with new chunks and values
    if (bytecodeUpdateController.GetCountBytecodeChuncksToInsert() == 0) return error("No bytecode has been added");

    setBound(newId);

    //apply the bytecode update controller
    if (!ApplyBytecodeUpdateController(bytecodeUpdateController)) error("failed to update the bytecode update controller");

    //bytecode has been updated: reupdate all maps
    if (!UpdateAllMaps()) error("failed to update all maps");

#ifdef XKSLANG_DEBUG_MODE
    //=============================================================================================================
    //validity check: make sure that a stage input variables are compatible with the previous stage output variables
    for (unsigned int iStage = 1; iStage < outputStages.size(); ++iStage)
    {
        XkslMixerOutputStage& stage = outputStages[iStage];
        XkslMixerOutputStage& previousStage = outputStages[iStage - 1];

        unsigned int stageCountInputVariables = (unsigned int)stage.listStageInputVariableInfo.size();
        if (stageCountInputVariables != previousStage.listStageOutputVariableInfo.size())
            return error("a stage has more input variables than the previous stage output variables");

        for (unsigned int k = 0; k < stageCountInputVariables; ++k)
        {
            const XkslMixerOutputStage::OutputStageIOVariable& stageInputVariable = stage.listStageInputVariableInfo[k];
            const XkslMixerOutputStage::OutputStageIOVariable& previousStageOutputVariable = previousStage.listStageOutputVariableInfo[k];

            if (stageInputVariable.globalStreamMemberIndex != previousStageOutputVariable.globalStreamMemberIndex)
                return error("a stage input variable does not match the previous stage's expected output variable");
            if (stageInputVariable.locationNum != previousStageOutputVariable.locationNum)
                return error("a stage input variable's location does not match the previous stage's output variable's expected location");
        }
    }
#endif

    //=============================================================================================================
    if (errorMessages.size() > 0) return false;
    return true;
}

