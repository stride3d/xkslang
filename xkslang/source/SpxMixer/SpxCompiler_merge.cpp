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


bool SpxCompiler::MergeShadersIntoBytecode(SpxCompiler& bytecodeToMerge, const vector<ShaderToMergeData>& listShadersToMerge, string allInstancesPrefixToAdd)
{
    //if (&bytecodeToMerge == this) {
    //    return error("Cannot merge a bytecode into its own code");
    //}

    if (listShadersToMerge.size() == 0) return true;

    //=============================================================================================================
    //Init destination bytecode hashmap (to compare types and consts hashmap id)
    unordered_map<uint32_t, pairIdPos> destinationBytecodeTypeHashMap;
    if (!this->BuildTypesAndConstsHashmap(destinationBytecodeTypeHashMap)) {
        return error("Merge shaders. Error building type and const hashmap");
    }

    //=============================================================================================================
    //=============================================================================================================
    //Merge all the shaders
    int newId = bound();

    vector<uint32_t> vecNamesAndDecorateToMerge;
    vector<uint32_t> vecXkslDecoratesToMerge;
    vector<uint32_t> vecTypesConstsAndVariablesToMerge;
    vector<uint32_t> vecFunctionsToMerge;
    vector<uint32_t> vecHeaderPropertiesToMerge;

    vector<spv::Id> finalRemapTable;
    finalRemapTable.resize(bytecodeToMerge.bound(), spvUndefinedId);
    //keep the list of merged Ids so that we can look for their name or decorate
    vector<bool> listAllNewIdMerged;
    vector<bool> listIdsWhereToAddNamePrefix;
    listAllNewIdMerged.resize(bytecodeToMerge.bound(), false);
    listIdsWhereToAddNamePrefix.resize(bytecodeToMerge.bound(), false);
    vector<pair<spv::Id, spv::Id>> listOverridenFunctionMergedToBeRemappedWithMergedFunctions;

    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        const ShaderToMergeData& shaderToMergeData = listShadersToMerge[is];
        ShaderClassData* shaderToMerge = shaderToMergeData.shader;

        if (shaderToMerge->bytecodeSource != &bytecodeToMerge)
        {
            error(string("Shader: ") + shaderToMerge->GetName() + string(" does not belong to the source to merge"));
            return false;
        }
        shaderToMerge->tmpClonedShader = nullptr;
        bool instantiateTheShader = shaderToMergeData.instantiateShader;

        //check that the shader does not already exist in the destination bytecode
        string shaderToMergeFinalName = shaderToMerge->GetName();
        if (instantiateTheShader) shaderToMergeFinalName = allInstancesPrefixToAdd + shaderToMergeFinalName;
        if (this->GetShaderByName(shaderToMergeFinalName) != nullptr) {
            return error(string("A shader already exists in destination bytecode with the name: ") + shaderToMergeFinalName);
        }

        //=============================================================================================================
        //=============================================================================================================
        //merge all types and variables declared by the shader
        for (unsigned int t = 0; t < shaderToMerge->shaderTypesList.size(); ++t)
        {
            ShaderTypeData* shaderTypeToMerge = shaderToMerge->shaderTypesList[t];

            TypeInstruction* type = shaderTypeToMerge->type;
            TypeInstruction* pointerToType = shaderTypeToMerge->pointerToType;
            VariableInstruction* variable = shaderTypeToMerge->variable;

#ifdef XKSLANG_DEBUG_MODE
            if (finalRemapTable[type->GetId()] != spvUndefinedId) error(string("id: ") + to_string(type->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[pointerToType->GetId()] != spvUndefinedId) error(string("id: ") + to_string(pointerToType->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[variable->GetId()] != spvUndefinedId) error(string("id: ") + to_string(variable->GetId()) + string(" has already been remapped"));
#endif

            finalRemapTable[type->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, type->GetBytecodeStartPosition());
            finalRemapTable[pointerToType->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, pointerToType->GetBytecodeStartPosition());
            finalRemapTable[variable->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, variable->GetBytecodeStartPosition());

            //don't update the name of non-instantiated merged shaders
            if (instantiateTheShader) {
                listIdsWhereToAddNamePrefix[type->GetId()] = true;
                listIdsWhereToAddNamePrefix[pointerToType->GetId()] = true;
                listIdsWhereToAddNamePrefix[variable->GetId()] = true;
            }
        }

        //Add the shader type declaration
        {
            const spv::Id resultId = shaderToMerge->GetId();

#ifdef XKSLANG_DEBUG_MODE
            if (finalRemapTable[resultId] != spvUndefinedId) error(string("id: ") + to_string(resultId) + string(" has already been remapped"));
#endif

            finalRemapTable[resultId] = newId++;

            //bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, shaderToMerge->GetBytecodeStartPosition());
            ///rebuild a OpTypeXlslShaderClass instruction with the shaderName updated with the prefix
            spv::Instruction shaderTypeInstrucion(resultId, 0, spv::OpTypeXlslShaderClass);

            spv::XkslPropertyEnum shaderType = instantiateTheShader ? spv::XkslPropertyEnum::ShaderTypeInstance : spv::XkslPropertyEnum::ShaderTypeBase;
            shaderTypeInstrucion.addImmediateOperand(shaderType);
            shaderTypeInstrucion.addImmediateOperand(shaderToMerge->countGenerics);
            shaderTypeInstrucion.addStringOperand(shaderToMergeFinalName.c_str());
            shaderTypeInstrucion.addStringOperand(shaderToMerge->shaderOriginalTypeName.c_str());  //the original type name always stays the same
            shaderTypeInstrucion.dump(vecTypesConstsAndVariablesToMerge);

            if (instantiateTheShader) {
                listIdsWhereToAddNamePrefix[resultId] = true; //to update the new shader declaration name
            }
        }

        //=============================================================================================================
        //=============================================================================================================
        //add all functions' instructions declared by the shader
        for (unsigned int t = 0; t < shaderToMerge->functionsList.size(); ++t)
        {
            FunctionInstruction* functionToMerge = shaderToMerge->functionsList[t];
            //finalRemapTable[functionToMerge->id] = newId++; //done below

            if (instantiateTheShader) {
                listIdsWhereToAddNamePrefix[functionToMerge->GetId()] = true;
            }

            //If the function is already overriden by another function: we'll update the link in the cloned functions as well
            if (functionToMerge->GetOverridingFunction() != nullptr)
            {
                listOverridenFunctionMergedToBeRemappedWithMergedFunctions.push_back(pair<spv::Id, spv::Id>(functionToMerge->GetId(), functionToMerge->GetOverridingFunction()->GetId()));
            }

            //For each instructions within the functions bytecode: Remap their results IDs
            {
                unsigned int start = functionToMerge->GetBytecodeStartPosition();
                const unsigned int end = functionToMerge->GetBytecodeEndPosition();
                while (start < end)
                {
                    unsigned int wordCount = bytecodeToMerge.asWordCount(start);
                    spv::Op opCode = bytecodeToMerge.asOpCode(start);

                    unsigned word = start + 1;

                    // Read type and result ID from instruction desc table
                    if (spv::InstructionDesc[opCode].hasType()) {
                        word++;  //spv::Id typeId = bytecodeToMerge.asId(word++);
                    }

                    if (spv::InstructionDesc[opCode].hasResult())
                    {
                        spv::Id resultId = bytecodeToMerge.asId(word++);
#ifdef XKSLANG_DEBUG_MODE
                        if (finalRemapTable[resultId] != spvUndefinedId) error(string("id: ") + to_string(resultId) + string(" has already been remapped"));
#endif
                        finalRemapTable[resultId] = newId++;
                    }

                    start += wordCount;
                }
            }

            //Copy all bytecode instructions from the function
            bytecodeToMerge.CopyInstructionToVector(vecFunctionsToMerge, functionToMerge->GetBytecodeStartPosition(), functionToMerge->GetBytecodeEndPosition());
        }
    } //end shaderToMerge loop

    {
        //update listAllNewIdMerged table (this table defines the name and decorate to fetch and merge)
        unsigned int len = (unsigned int)finalRemapTable.size();
        for (unsigned int i = 0; i < len; ++i)
        {
            if (finalRemapTable[i] != spvUndefinedId) listAllNewIdMerged[i] = true;
        }
    }

    //Populate vecNewShadersDecorationPossesingIds with some decorate instructions which might contains unmapped IDs
    vector<uint32_t> vecXkslDecorationsPossesingIds;
    {
        unsigned int start = bytecodeToMerge.header_size;
        const unsigned int end = (unsigned int)bytecodeToMerge.spv.size();
        while (start < end)
        {
            unsigned int wordCount = bytecodeToMerge.asWordCount(start);
            spv::Op opCode = bytecodeToMerge.asOpCode(start);

            switch (opCode)
            {
            case spv::OpShaderInheritance:
            case spv::OpShaderCompositionDeclaration:
            case spv::OpShaderCompositionInstance:
            {
                const spv::Id id = bytecodeToMerge.asId(start + 1);
                if (listAllNewIdMerged[id])
                {
                    bytecodeToMerge.CopyInstructionToVector(vecXkslDecorationsPossesingIds, start);
                }
                break;
            }

            case spv::OpTypeXlslShaderClass:
            {
                //all done at this point
                start = end;
                break;
            }
            }

            start += wordCount;
        }
    }

    //=============================================================================================================
    //=============================================================================================================
    //Check for all unmapped Ids called within the shader types/consts instructions that we have to merge
    vector<uint32_t> bytecodeWithExtraTypesToMerge;
    {
        //Init by checking all types instructions for unmapped IDs
        vector<uint32_t> bytecodeToCheckForUnmappedIds;
        bytecodeToCheckForUnmappedIds.insert(bytecodeToCheckForUnmappedIds.end(), vecTypesConstsAndVariablesToMerge.begin(), vecTypesConstsAndVariablesToMerge.end());
        bytecodeToCheckForUnmappedIds.insert(bytecodeToCheckForUnmappedIds.end(), vecFunctionsToMerge.begin(), vecFunctionsToMerge.end());
        bytecodeToCheckForUnmappedIds.insert(bytecodeToCheckForUnmappedIds.end(), vecXkslDecorationsPossesingIds.begin(), vecXkslDecorationsPossesingIds.end());
        vector<pairIdPos> listUnmappedIdsToProcess;  //unmapped ids and their pos in the bytecode to merge

        spv::Op opCode;
        unsigned int wordCount;
        vector<spv::Id> listIds;
        spv::Id typeId, resultId;
        unsigned int listIdsLen;
        unsigned int start = 0;
        const unsigned int end = (unsigned int)bytecodeToCheckForUnmappedIds.size();
        string errorMsg;
        while (start < end)
        {
            listIds.clear();
            if (!parseInstruction(bytecodeToCheckForUnmappedIds, start, opCode, wordCount, typeId, resultId, listIds, errorMsg)) {
                error(errorMsg);
                return error("Merge shaders. Error parsing bytecodeToCheckForUnmappedIds instructions");
            }

            if (typeId != spvUndefinedId) listIds.push_back(typeId);
            listIdsLen = (unsigned int)listIds.size();
            for (unsigned int i = 0; i < listIdsLen; ++i)
            {
                const spv::Id id = listIds[i];
                if (finalRemapTable[id] == spvUndefinedId)
                {
                    listUnmappedIdsToProcess.push_back(pairIdPos(id, -1));
                }
            }

            start += wordCount;
        }

        //TypeData* typeToMerge;
        //ConstData* constToMerge;
        //VariableData* variableToAccess;
        while (listUnmappedIdsToProcess.size() > 0)
        {
            pairIdPos& unmappedIdPos = listUnmappedIdsToProcess.back();
            const spv::Id unmappedId = unmappedIdPos.first;
            if (finalRemapTable[unmappedId] != spvUndefinedId)
            {
                listUnmappedIdsToProcess.pop_back();
                continue;
            }

            //Find the position in the code to merge where the unmapped IDs is defined
            ObjectInstructionBase* objectFromUnmappedId = bytecodeToMerge.GetObjectById(unmappedId);
            if (objectFromUnmappedId == nullptr) return error(string("Merge shaders. No object is defined for the unmapped id: ") + to_string(unmappedId));

            bool mappingResolved = false;
            bool mergeTypeOrConst = false;
            //uint32_t instructionPos = object->GetBytecodeStartPosition();
            switch (objectFromUnmappedId->GetKind())
            {
            case ObjectInstructionTypeEnum::Const:
            case ObjectInstructionTypeEnum::Type:
            {
                mergeTypeOrConst = true;

                uint32_t typeHash = bytecodeToMerge.hashType(objectFromUnmappedId->GetBytecodeStartPosition());
                auto hashTypePosIt = destinationBytecodeTypeHashMap.find(typeHash);
                if (hashTypePosIt != destinationBytecodeTypeHashMap.end())
                {
                    spv::Id idOfSameTypeFromDestinationBytecode = hashTypePosIt->second.first;

                    if (idOfSameTypeFromDestinationBytecode == spvUndefinedId)
                    {
                        return error(string("Merge shaders. hashmap refers to an invalid Id"));
                    }

                    //The type already exists in the destination bytecode, we can simply remap to it
                    mappingResolved = true;
                    finalRemapTable[unmappedId] = idOfSameTypeFromDestinationBytecode;
                }
                break;
            }

            case ObjectInstructionTypeEnum::Variable:
            {
                //this is a variable from destination bytecode, we retrieve its id in the destination bytecode using its declaration name
                VariableInstruction* variable = this->GetVariableByName(objectFromUnmappedId->GetName());
                if (variable == nullptr)
                    return error(string("Merge shaders. No variable exists in destination bytecode with the name: ") + objectFromUnmappedId->GetName());

                //remap the unmapped ID to the destination variable
                mappingResolved = true;
                finalRemapTable[unmappedId] = variable->GetId();
                break;
            }

            case ObjectInstructionTypeEnum::Function:
            {
                //this is a function from destination bytecode, we retrieve its id in the destination bytecode using its declaration name and shader owner
                if (objectFromUnmappedId->GetShaderOwner() == nullptr)
                    return error(string("The function does not have a shader owner: ") + objectFromUnmappedId->GetName());

                ShaderClassData* shader = this->GetShaderByName(objectFromUnmappedId->GetShaderOwner()->GetName());
                if (shader == nullptr)
                    return error(string("No shader exists in destination bytecode with the name: ") + objectFromUnmappedId->GetShaderOwner()->GetName());
                FunctionInstruction* function = shader->GetFunctionByName(objectFromUnmappedId->GetName());
                if (function == nullptr)
                    return error(string("Shader \"") + shader->GetName() + string("\" does not have a function named \"") + objectFromUnmappedId->GetName() + string("\""));

                //remap the unmapped ID to the destination variable
                mappingResolved = true;
                finalRemapTable[unmappedId] = function->GetId();
                break;
            }

            case ObjectInstructionTypeEnum::Shader:
            {
                //This is a shader from destination bytecode, we simply retrieve its Ids
                ShaderClassData* shader = this->GetShaderByName(objectFromUnmappedId->GetName());
                if (shader == nullptr)
                    return error(string("Merge shaders. No shader exists in destination bytecode with the name: ") + objectFromUnmappedId->GetName());

                //remap the unmapped ID to the destination variable
                mappingResolved = true;
                finalRemapTable[unmappedId] = shader->GetId();
                break;
            }

            case ObjectInstructionTypeEnum::HeaderProperty:
            {
                //import an external lib if we don't already have it
                HeaderPropertyInstruction* hearderProp = this->GetHeaderPropertyInstructionByOpCodeAndName(objectFromUnmappedId->GetOpCode(), objectFromUnmappedId->GetName());
                if (hearderProp != nullptr)
                    finalRemapTable[unmappedId] = hearderProp->GetId();
                else
                {
                    bytecodeToMerge.CopyInstructionToVector(vecHeaderPropertiesToMerge, objectFromUnmappedId->GetBytecodeStartPosition());
                    finalRemapTable[unmappedId] = newId++;
                    listAllNewIdMerged[unmappedId] = true;
                }

                mappingResolved = true;
                break;
            }

            default:
                return error(string("Merge shaders. Invalid object. unable to remap unmapped id: ") + to_string(unmappedId));
            }

            if (mappingResolved)
            {
                listUnmappedIdsToProcess.pop_back();
            }
            else
            {
                if (mergeTypeOrConst)
                {
                    //The type doesn't exist yet, we will copy the full instruction, but only after checking that all depending IDs are mapped as well
                    bytecodeToCheckForUnmappedIds.clear();
                    bytecodeToMerge.CopyInstructionToVector(bytecodeToCheckForUnmappedIds, objectFromUnmappedId->GetBytecodeStartPosition());
                    listIds.clear();
                    if (!parseInstruction(bytecodeToCheckForUnmappedIds, 0, opCode, wordCount, typeId, resultId, listIds, errorMsg)) {
                        error(errorMsg);
                        return error("Error parsing bytecodeToCheckForUnmappedIds instructions");
                    }

                    if (typeId != spvUndefinedId) listIds.push_back(typeId);
                    listIdsLen = (unsigned int)listIds.size();
                    bool canAddTheInstruction = true;
                    for (unsigned int i = 0; i < listIdsLen; ++i)
                    {
                        const spv::Id anotherId = listIds[i];
#ifdef XKSLANG_DEBUG_MODE
                        if (anotherId == unmappedId) return error(string("anotherId == unmappedId: ") + to_string(anotherId) + string(". This should be impossible (bytecode is invalid)"));
#endif
                        if (finalRemapTable[anotherId] == spvUndefinedId)
                        {
                            //we add anotherId to the list of Ids to process (we're depending on it)
                            listUnmappedIdsToProcess.push_back(pairIdPos(anotherId, -1));
                            canAddTheInstruction = false;
                        }
                    }

                    if (canAddTheInstruction)
                    {
                        //the instruction is not depending on another unmapped IDs anymore, we can copy it
                        finalRemapTable[unmappedId] = newId++;
                        listUnmappedIdsToProcess.pop_back();
                        bytecodeToMerge.CopyInstructionToVector(bytecodeWithExtraTypesToMerge, objectFromUnmappedId->GetBytecodeStartPosition());

                        listAllNewIdMerged[unmappedId] = true;
                    }
                }
            }
        }  //end while (listUnmappedIdsToProcess.size() > 0)
    }  //end block

       //Add the extra types we merged at the beginning of our vec of type/const/variable
    if (bytecodeWithExtraTypesToMerge.size() > 0)
    {
        vecTypesConstsAndVariablesToMerge.insert(vecTypesConstsAndVariablesToMerge.begin(), bytecodeWithExtraTypesToMerge.begin(), bytecodeWithExtraTypesToMerge.end());
    }

    //=============================================================================================================
    //=============================================================================================================
    // get all name, decoration and XKSL data for the new IDs we need to merge
    {
        unsigned int start = bytecodeToMerge.header_size;
        const unsigned int end = (unsigned int)bytecodeToMerge.spv.size();
        while (start < end)
        {
            unsigned int wordCount = bytecodeToMerge.asWordCount(start);
            spv::Op opCode = bytecodeToMerge.asOpCode(start);

            switch (opCode)
            {
            case spv::OpName:
            {
                //We rename the OpName with the prefix (for debug purposes)
                const spv::Id id = bytecodeToMerge.asId(start + 1);
                if (listAllNewIdMerged[id])
                {
                    if (listIdsWhereToAddNamePrefix[id])
                    {
                        //disassemble and reassemble the instruction with a name updated with the prefix
                        string strUpdatedName(allInstancesPrefixToAdd + bytecodeToMerge.literalString(start + 2));
                        spv::Instruction nameInstr(opCode);
                        nameInstr.addIdOperand(id);
                        nameInstr.addStringOperand(strUpdatedName.c_str());
                        nameInstr.dump(vecNamesAndDecorateToMerge);
                    }
                    else
                    {
                        bytecodeToMerge.CopyInstructionToVector(vecNamesAndDecorateToMerge, start);
                    }
                }
                break;
            }

            case spv::OpMemberName:
            case spv::OpDecorate:
            case spv::OpMemberDecorate:
            {
                const spv::Id id = bytecodeToMerge.asId(start + 1);
                if (listAllNewIdMerged[id])
                {
                    bytecodeToMerge.CopyInstructionToVector(vecNamesAndDecorateToMerge, start);
                }
                break;
            }

            case spv::OpDeclarationName:
            {
                //We update the declaration name only for shader classes and for variables
                const spv::Id id = bytecodeToMerge.asId(start + 1);
                if (listAllNewIdMerged[id])
                {
                    bool addPrefix = false;
                    if (listIdsWhereToAddNamePrefix[id])
                    {
                        ShaderClassData* shader = bytecodeToMerge.GetShaderById(id);
                        if (shader != nullptr) addPrefix = true;
                        else
                        {
                            VariableInstruction* variable = bytecodeToMerge.GetVariableById(id);
                            if (variable != nullptr) addPrefix = true;
                        }
                    }

                    if (addPrefix)
                    {
                        //disassemble and reassemble the instruction with a name updated with the prefix
                        string strUpdatedName(allInstancesPrefixToAdd + bytecodeToMerge.literalString(start + 2));
                        spv::Instruction nameInstr(opCode);
                        nameInstr.addIdOperand(id);
                        nameInstr.addStringOperand(strUpdatedName.c_str());
                        nameInstr.dump(vecXkslDecoratesToMerge);
                    }
                    else
                    {
                        bytecodeToMerge.CopyInstructionToVector(vecXkslDecoratesToMerge, start);
                    }
                }
                break;
            }

            case spv::OpBelongsToShader:
            case spv::OpShaderInheritance:
            case spv::OpShaderCompositionDeclaration:
            case spv::OpShaderCompositionInstance:
            case spv::OpMethodProperties:
            case spv::OpGSMethodProperties:
            case spv::OpMemberProperties:
            case spv::OpMemberAttribute:
            case spv::OpCBufferProperties:
            case spv::OpMemberSemanticName:
            case spv::OpSemanticName:
            case spv::OpMemberLinkName:
            case spv::OpLinkName:
            {
                const spv::Id id = bytecodeToMerge.asId(start + 1);
                if (listAllNewIdMerged[id])
                {
                    bytecodeToMerge.CopyInstructionToVector(vecXkslDecoratesToMerge, start);
                }
                break;
            }
            }

            start += wordCount;
        }
    }

    //=============================================================================================================
    //=============================================================================================================
    //remap IDs for all mergable types / variables / consts / functions / extImport
    if (vecHeaderPropertiesToMerge.size() > 0)
    {
        if (!remapAllIds(vecHeaderPropertiesToMerge, 0, (unsigned int)vecHeaderPropertiesToMerge.size(), finalRemapTable))
            return error("remapAllIds failed on vecHeaderPropertiesToMerge");
    }
    if (!remapAllIds(vecTypesConstsAndVariablesToMerge, 0, (unsigned int)vecTypesConstsAndVariablesToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecTypesConstsAndVariablesToMerge");
    if (!remapAllIds(vecXkslDecoratesToMerge, 0, (unsigned int)vecXkslDecoratesToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecXkslDecoratesToMerge");
    if (!remapAllIds(vecNamesAndDecorateToMerge, 0, (unsigned int)vecNamesAndDecorateToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecNamesAndDecorateToMerge");
    if (!remapAllIds(vecFunctionsToMerge, 0, (unsigned int)vecFunctionsToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecFunctionsToMerge");
    //vecFunctionsToMerge.processOnFullBytecode(
    //    spx_inst_fn_nop,
    //    [&](spv::Id& id)
    //    {
    //        spv::Id newId = finalRemapTable[id];
    //        if (newId == spvUndefinedId) error(string("Invalid remapper Id: ") + to_string(id));
    //        else id = newId;
    //    }
    //);

    //=============================================================================================================
    //=============================================================================================================
    //merge all types / variables / consts in the current bytecode
    setBound(newId);

    //Find the best positions in the bytecode where to merge the new stuff
    unsigned int posToInsertNewHeaderProrerties = header_size;
    unsigned int posToInsertNewNamesAndXkslDecorates = header_size;
    unsigned int posToInsertNewTypesAndConsts = header_size;
    unsigned int posToInsertNewFunctions = (unsigned int)spv.size();
    bool firstFunc = true;
    bool firstType = true;
    for (auto ito = listAllObjects.begin(); ito != listAllObjects.end(); ito++)
    {
        ObjectInstructionBase* obj = *ito;
        if (obj == nullptr) continue;

        switch (obj->GetKind())
        {
        case ObjectInstructionTypeEnum::HeaderProperty:
            if (posToInsertNewHeaderProrerties < obj->GetBytecodeEndPosition())
            {
                posToInsertNewHeaderProrerties = obj->GetBytecodeEndPosition();
                if (posToInsertNewTypesAndConsts < posToInsertNewHeaderProrerties) posToInsertNewTypesAndConsts = posToInsertNewHeaderProrerties;
                if (posToInsertNewNamesAndXkslDecorates < posToInsertNewHeaderProrerties) posToInsertNewNamesAndXkslDecorates = posToInsertNewHeaderProrerties;
            }
            break;
        case ObjectInstructionTypeEnum::Type:
        case ObjectInstructionTypeEnum::Variable:
        case ObjectInstructionTypeEnum::Shader:
        case ObjectInstructionTypeEnum::Const:
            if (firstType || posToInsertNewNamesAndXkslDecorates > obj->GetBytecodeStartPosition())
            {
                posToInsertNewNamesAndXkslDecorates = obj->GetBytecodeStartPosition();
                if (posToInsertNewTypesAndConsts < posToInsertNewNamesAndXkslDecorates) posToInsertNewTypesAndConsts = posToInsertNewNamesAndXkslDecorates;
            }
            firstType = false;
            break;
        case ObjectInstructionTypeEnum::Function:
            if (firstFunc || posToInsertNewTypesAndConsts > obj->GetBytecodeStartPosition())
            {
                posToInsertNewTypesAndConsts = obj->GetBytecodeStartPosition();
            }
            firstFunc = false;
            break;
        }
    }
    if (posToInsertNewTypesAndConsts > posToInsertNewFunctions) posToInsertNewTypesAndConsts = posToInsertNewFunctions;
    if (posToInsertNewNamesAndXkslDecorates > posToInsertNewTypesAndConsts) posToInsertNewNamesAndXkslDecorates = posToInsertNewTypesAndConsts;
    if (posToInsertNewHeaderProrerties > posToInsertNewNamesAndXkslDecorates) posToInsertNewHeaderProrerties = posToInsertNewNamesAndXkslDecorates;

    unsigned int posToInsertNewNames = posToInsertNewNamesAndXkslDecorates;
    unsigned int posToInsertNewXkslDecorates = posToInsertNewNamesAndXkslDecorates;
    //We can refine posToInsertNewNames to put it behind all previous XkslDecorate, as long as we leave it between [posToInsertNewHeaderProrerties, posToInsertNewXkslDecorates]
    {
        unsigned int start = posToInsertNewHeaderProrerties;
        const unsigned int end = posToInsertNewXkslDecorates;
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode) {
            case spv::OpDeclarationName:
            case spv::OpShaderInheritance:
            case spv::OpBelongsToShader:
            case spv::OpShaderCompositionDeclaration:
            case spv::OpShaderCompositionInstance:
            case spv::OpMethodProperties:
            case spv::OpGSMethodProperties:
            case spv::OpMemberProperties:
            case spv::OpMemberAttribute:
            case spv::OpCBufferProperties:
            case spv::OpMemberSemanticName:
            case spv::OpSemanticName:
            case spv::OpMemberLinkName:
            case spv::OpLinkName:
            {
                posToInsertNewNames = start;
                start = end;
                break;
            }
            }
            start += wordCount;
        }
    }

    //=============================================================================================================
    // merge all data in the destination bytecode
    //merge functions
    spv.insert(spv.begin() + posToInsertNewFunctions, vecFunctionsToMerge.begin(), vecFunctionsToMerge.end());
    //Merge types and variables (we need to merge new types AFTER all previous types)
    spv.insert(spv.begin() + posToInsertNewTypesAndConsts, vecTypesConstsAndVariablesToMerge.begin(), vecTypesConstsAndVariablesToMerge.end());
    //Merge names and decorates
    spv.insert(spv.begin() + posToInsertNewXkslDecorates, vecXkslDecoratesToMerge.begin(), vecXkslDecoratesToMerge.end());
    spv.insert(spv.begin() + posToInsertNewNames, vecNamesAndDecorateToMerge.begin(), vecNamesAndDecorateToMerge.end());
    //merge ext import
    spv.insert(spv.begin() + posToInsertNewHeaderProrerties, vecHeaderPropertiesToMerge.begin(), vecHeaderPropertiesToMerge.end());

    //destination bytecode has been updated: reupdate all maps
    UpdateAllMaps();

    //Set reference between shaders merged and the clone shaders from destination bytecode
    vector<ShaderClassData*> listMergedShaders;
    for (unsigned int is = 0; is < listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is].shader;
        spv::Id clonedShaderId = finalRemapTable[shaderToMerge->GetId()];
        ShaderClassData* clonedShader = this->GetShaderById(clonedShaderId);
        if (clonedShader == nullptr) return error(string("Cannot retrieve the merged shader: ") + to_string(clonedShaderId));
        shaderToMerge->tmpClonedShader = clonedShader;
        listMergedShaders.push_back(clonedShader);
    }

    //update overriden references to merged functions
    for (unsigned int i = 0; i < listOverridenFunctionMergedToBeRemappedWithMergedFunctions.size(); ++i)
    {
        spv::Id functionOverridenId = finalRemapTable[listOverridenFunctionMergedToBeRemappedWithMergedFunctions[i].first];
        spv::Id functionOverridingId = finalRemapTable[listOverridenFunctionMergedToBeRemappedWithMergedFunctions[i].second];
        FunctionInstruction* functionOverriden = this->GetFunctionById(functionOverridenId);
        FunctionInstruction* functionOverriding = this->GetFunctionById(functionOverridingId);
        if (functionOverriden == nullptr) return error(string("Cannot retrieve the merged function: ") + to_string(functionOverridenId));
        if (functionOverriding == nullptr) return error(string("Cannot retrieve the merged function: ") + to_string(functionOverridingId));
        functionOverriden->SetOverridingFunction(functionOverriding);
    }

    if (!CheckIfAnyNewCompositionGetOverridenByExistingOnes(listMergedShaders))
    {
        return error("Failed to check if some new compositions get overriden by the existing ones");
    }

    if (errorMessages.size() > 0) return false;
    return true;
}