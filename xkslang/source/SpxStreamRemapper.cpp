//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "SPIRV/doc.h"
//#include "SPIRV/disassemble.h"
//#include "SPIRV/SPVRemapper.h"

#include "SpxStreamRemapper.h"

using namespace std;
using namespace xkslang;

//===================================================================================================//
static vector<string> staticErrorMessages;
static void copyStaticErrorMessagesTo(vector<string>& list)
{
    list.insert(list.end(), staticErrorMessages.begin(), staticErrorMessages.end());
}

void SpxStreamRemapper::copyMessagesTo(vector<string>& list)
{
    list.insert(list.end(), errorMessages.begin(), errorMessages.end());
}

void SpxStreamRemapper::error(const string& txt) const
{
    //we use a static vector first because we override a function defined as const, and parent class is calling this function >_<
    staticErrorMessages.push_back(txt);
}

bool SpxStreamRemapper::error(const string& txt)
{
    errorMessages.push_back(txt);
    return false;
}

static const auto spx_inst_fn_nop = [](spv::Op, unsigned) { return false; };
static const auto spx_op_fn_nop = [](spv::Id&) {};
//===================================================================================================//

SpxStreamRemapper::SpxStreamRemapper(int verbose) : spirvbin_t(verbose)
{
    staticErrorMessages.clear(); //clear the list of error messages that we will receive from the parent class
    status = SpxRemapperStatusEnum::WaitingForMixin;
}

SpxStreamRemapper::~SpxStreamRemapper()
{
    ReleaseAllMaps();
}

SpxStreamRemapper* SpxStreamRemapper::Clone()
{
    SpxStreamRemapper* clonedSpxRemapper = new SpxStreamRemapper();
    clonedSpxRemapper->spv = this->spv;
    clonedSpxRemapper->listAllObjects.resize(listAllObjects.size(), nullptr);

    for (unsigned int i=0; i<listAllObjects.size(); ++i)
    {
        ObjectInstructionBase* obj = listAllObjects[i];
        if (obj != nullptr)
        {
            ObjectInstructionBase* clonedObj = obj->CloneBasicData();
            clonedObj->bytecodeSource = clonedSpxRemapper;
            clonedSpxRemapper->listAllObjects[i] = clonedObj;
        }
    }

    for (unsigned int i = 0; i < listAllObjects.size(); ++i)
    {
        ObjectInstructionBase* obj = listAllObjects[i];
        if (obj == nullptr) continue;

        ObjectInstructionBase* clonedObj = clonedSpxRemapper->listAllObjects[i];
        switch (obj->GetKind())
        {
            case ObjectInstructionTypeEnum::Type:
            {
                TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
                if (type->GetTypePointed() != nullptr)
                {
                    TypeInstruction* clonedType = dynamic_cast<TypeInstruction*>(clonedObj);
                    TypeInstruction* pointedType = clonedSpxRemapper->GetTypeById(type->GetTypePointed()->GetId());
                    clonedType->SetTypePointed(pointedType);
                }
                break;
            }
            case ObjectInstructionTypeEnum::Variable:
            {
                VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
                if (variable->GetTypePointed() != nullptr)
                {
                    VariableInstruction* clonedVariable = dynamic_cast<VariableInstruction*>(clonedObj);
                    TypeInstruction* pointedType = clonedSpxRemapper->GetTypeById(variable->GetTypePointed()->GetId());
                    clonedVariable->SetTypePointed(pointedType);
                }
                break;
            }
            case ObjectInstructionTypeEnum::Function:
            {
                FunctionInstruction* function = dynamic_cast<FunctionInstruction*>(obj);
                FunctionInstruction* clonedFunction = dynamic_cast<FunctionInstruction*>(clonedObj);
                if (function->GetOverridingFunction() != nullptr)
                {
                    FunctionInstruction* pointedFunction = clonedSpxRemapper->GetFunctionById(function->GetOverridingFunction()->GetId());
                    clonedFunction->SetOverridingFunction(pointedFunction);
                }
                if (function->GetShaderOwner() != nullptr)
                {
                    ShaderClassData* shaderOwner = clonedSpxRemapper->GetShaderById(function->GetShaderOwner()->GetId());
                    clonedFunction->SetShaderOwner(shaderOwner);
                }
                break;
            }
            case ObjectInstructionTypeEnum::Shader:
            {
                ShaderClassData* shader = dynamic_cast<ShaderClassData*>(obj);
                ShaderClassData* clonedShader = dynamic_cast<ShaderClassData*>(clonedObj);
                shader->tmpClonedShader = clonedShader;
                for (auto it = shader->parentsList.begin(); it != shader->parentsList.end(); it++)
                {
                    ShaderClassData* parent = clonedSpxRemapper->GetShaderById((*it)->GetId());
                    clonedShader->AddParent(parent);
                }
                for (auto it = shader->functionsList.begin(); it != shader->functionsList.end(); it++)
                {
                    FunctionInstruction* function = clonedSpxRemapper->GetFunctionById((*it)->GetId());
                    clonedShader->AddFunction(function);
                }
                for (auto it = shader->shaderTypesList.begin(); it != shader->shaderTypesList.end(); it++)
                {
                    ShaderTypeData* type = *it;
                    TypeInstruction* clonedType = clonedSpxRemapper->GetTypeById(type->type->GetId());
                    TypeInstruction* clonedTypePointer = clonedSpxRemapper->GetTypeById(type->pointerToType->GetId());
                    VariableInstruction* clonedVariable = clonedSpxRemapper->GetVariableById(type->variable->GetId());
                    ShaderTypeData* clonedShaderType = new ShaderTypeData(clonedType, clonedTypePointer, clonedVariable);
                    clonedShader->AddShaderType(clonedShaderType);
                }
                break;
            }
        }
    }

    for (auto it = mapDeclarationName.begin(); it != mapDeclarationName.end(); it++)
    {
        clonedSpxRemapper->mapDeclarationName[it->first] = it->second;
    }

    for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); it++)
    {
        ShaderClassData* shaderToClone = *it;
        ShaderClassData* clonedShader = clonedSpxRemapper->GetShaderById(shaderToClone->GetId());
        clonedSpxRemapper->vecAllShaders.push_back(clonedShader);

        for (unsigned int i = 0; i < shaderToClone->compositionsList.size(); ++i)
        {
            ShaderComposition& compositionToClone = shaderToClone->compositionsList[i];
            ShaderClassData* shaderType = compositionToClone.shaderType == nullptr? nullptr: clonedSpxRemapper->GetShaderById(compositionToClone.shaderType->GetId());
            ShaderComposition clonedComposition(compositionToClone.compositionShaderId, compositionToClone.compositionShaderOwner, shaderType, compositionToClone.variableName,
                compositionToClone.isArray, compositionToClone.status);
            //clonedComposition.instantiatedShader = compositionToClone.instantiatedShader == nullptr ? nullptr : clonedSpxRemapper->GetShaderById(compositionToClone.instantiatedShader->GetId());
            clonedShader->AddComposition(clonedComposition);
        }
    }

    for (auto it = vecAllShaderFunctions.begin(); it != vecAllShaderFunctions.end(); it++)
    {
        FunctionInstruction* function = clonedSpxRemapper->GetFunctionById((*it)->GetId());
        clonedSpxRemapper->vecAllShaderFunctions.push_back(function);
    }

    clonedSpxRemapper->status = status;
    for (auto it = idPosR.begin(); it != idPosR.end(); it++)
        clonedSpxRemapper->idPosR[it->first] = it->second;
    for (auto it = errorMessages.begin(); it != errorMessages.end(); it++)
        clonedSpxRemapper->errorMessages.push_back(*it);
    
    return clonedSpxRemapper;
}

bool SpxStreamRemapper::RemoveShaderAndAllData(ShaderClassData* shaderToRemove, vector<range_t>& vecStripRanges)
{
    bool shaderFound = false;
    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
    {
        ShaderClassData* shader = *itsh;
        if (shader == shaderToRemove)
        {
            shaderFound = true;
            iter_swap(itsh, vecAllShaders.end() - 1);
            vecAllShaders.pop_back();
            break;
        }
    }
    if (!shaderFound) return error(string("Failed to find the shader to remove: ") + shaderToRemove->GetName());

    //remove all functions belonging to the shader
    {
        unsigned int countFunctionsRemoved = 0;
        for (int i = 0; i < vecAllShaderFunctions.size(); ++i)
        {
            FunctionInstruction* function = vecAllShaderFunctions[i];
            if (function->shaderOwner == shaderToRemove)
            {
                countFunctionsRemoved++;

                if (i < vecAllShaderFunctions.size() - 1)
                    vecAllShaderFunctions[i] = vecAllShaderFunctions[vecAllShaderFunctions.size() - 1];
                vecAllShaderFunctions.pop_back();

                stripInst(vecStripRanges, function->GetBytecodeStartPosition(), function->GetBytecodeEndPosition());
                spv::Id id = function->GetId();
                delete listAllObjects[id];
                listAllObjects[id] = nullptr;
            }
        }
        if (countFunctionsRemoved != shaderToRemove->GetCountFunctions())
            return error(string("Discrepancy between shader list of functions and list of all shader functions for shader: ") + shaderToRemove->GetName());
    }

    //remove all types belonging to the shader
    {
        for (unsigned int t = 0; t < shaderToRemove->shaderTypesList.size(); ++t)
        {
            ShaderTypeData* shaderTypeToMerge = shaderToRemove->shaderTypesList[t];

            spv::Id id;
            TypeInstruction* type = shaderTypeToMerge->type;
            TypeInstruction* pointerToType = shaderTypeToMerge->pointerToType;
            VariableInstruction* variable = shaderTypeToMerge->variable;

            stripInst(vecStripRanges, type->GetBytecodeStartPosition(), type->GetBytecodeEndPosition());
            id = type->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;

            stripInst(vecStripRanges, pointerToType->GetBytecodeStartPosition(), pointerToType->GetBytecodeEndPosition());
            id = pointerToType->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;

            stripInst(vecStripRanges, variable->GetBytecodeStartPosition(), variable->GetBytecodeEndPosition());
            id = variable->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;
        }
    }

    //remove the shader's bytecode and object
    {
        stripInst(vecStripRanges, shaderToRemove->GetBytecodeStartPosition());
        spv::Id id = shaderToRemove->GetId();
        delete listAllObjects[id];
        listAllObjects[id] = nullptr;
    }

    return true;
}

void SpxStreamRemapper::ReleaseAllMaps()
{
    int size = listAllObjects.size();
    for (int i = 0; i < size; ++i)
    {
        if (listAllObjects[i] != nullptr) delete listAllObjects[i];
    }

    listAllObjects.clear();
    vecAllShaders.clear();
    vecAllShaderFunctions.clear();
    mapDeclarationName.clear();
}

bool SpxStreamRemapper::MixWithShadersFromBytecode(const SpxBytecode& sourceBytecode, const vector<string>& shaders)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    if (shaders.size() == 0) {
        return error("List of shader is empty");
    }

    //=============================================================================================================
    // Init the bytecode with a default header
    if (spv.size() == 0)
    {
        //Initialize a default header
        if (!InitDefaultHeader()) {
            return error("Failed to initialize a default header");
        }

        if (!this->BuildAllMaps()){
            return error("Error parsing the bytecode after having initialized the default header");
        }
    }

    //=============================================================================================================
    //parse the bytecode to merge
    SpxStreamRemapper bytecodeToMerge;
    if (!bytecodeToMerge.SetBytecode(sourceBytecode)) return false;

    bool res = bytecodeToMerge.BuildAllMaps();
    if (!res) {
        bytecodeToMerge.copyMessagesTo(errorMessages);
        return error(string("Error parsing the bytecode: ") + sourceBytecode.GetName());
    }

    for (auto itsh = bytecodeToMerge.vecAllShaders.begin(); itsh != bytecodeToMerge.vecAllShaders.end(); itsh++)
        (*itsh)->flag1 = 0;

    //=============================================================================================================
    //=============================================================================================================
    //Get the list of all shaders we want to merge
    vector<ShaderClassData*> listShadersToMerge;
    for (unsigned int is = 0; is < shaders.size(); ++is)
    {
        const string& shaderName = shaders[is];
        ShaderClassData* shaderToMerge = bytecodeToMerge.GetShaderByName(shaderName);
        if (shaderToMerge == nullptr) {
            return error(string("Shader not found: ") + shaderName);
        }

        //Get the shader parents
        vector<ShaderClassData*> listShadersFromSameFamily;
        bytecodeToMerge.GetShaderFamilyTreeWithParentAndCompositionType(shaderToMerge, listShadersFromSameFamily);

        for (unsigned int i = 0; i < listShadersFromSameFamily.size(); ++i)
        {
            ShaderClassData* shaderFromFamily = listShadersFromSameFamily[i];
            if (shaderFromFamily->flag1 == 0)
            {
                shaderFromFamily->flag1 = 1;

                //We don't add a shader if it already exists in the destination bytecode
                if (this->GetShaderByName(shaderFromFamily->GetName()) == nullptr)
                {
                    listShadersToMerge.push_back(shaderFromFamily);
                }
            }
        }
    }
    if (listShadersToMerge.size() == 0){
        status = SpxRemapperStatusEnum::WaitingForMixin;
        return true;
    }

    if (!MergeShadersIntoBytecode(bytecodeToMerge, listShadersToMerge, ""))
    {
        return false;
    }

    //=============================================================================================================
    //retrieve the merged shaders from the destination bytecode
    vector<ShaderClassData*> listShadersMerged;
    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is];
        ShaderClassData* shaderMerged = GetShaderByName(shaderToMerge->GetName());
        if (shaderMerged == nullptr) return error(string("Cannot retrieve the shader: ") + shaderToMerge->GetName());
        listShadersMerged.push_back(shaderMerged);
    }

    if (!ProcessOverrideAfterMixingNewShaders(listShadersMerged))
    {
        return error("Failed to process overrides after mixin new shaders");
    }
    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}

/*
bool SpxStreamRemapper::MixWithBytecode(const SpxBytecode& bytecode)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    //=============================================================================================================
    //=============================================================================================================
    //Merge or set the bytecode
    vector<ShaderClassData*> listShadersMerged;
    if (spv.size() == 0)
    {
        //just copy the full code into the remapper
        if (!SetBytecode(bytecode))
            return error(string("Error setting the bytecode: ") + bytecode.GetName());

        if (!BuildAllMaps())
            return error("Error building the bytecode data map");

        for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
            listShadersMerged.push_back(*itsh);
    }
    else
    {
        //merge the new bytecode into current one
        if (!MergeAllNewShadersFromBytecode(bytecode, listShadersMerged))
            return error(string("Error merging the shaders from the bytecode: ") + bytecode.GetName());
    }

    if (!ProcessOverrideAfterMixingNewShaders(listShadersMerged))
    {
        return error("Failed to process overrides after mixin new shaders");
    }
    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}
*/

bool SpxStreamRemapper::ProcessOverrideAfterMixingNewShaders(vector<ShaderClassData*>& listNewShaders)
{
    if (!ValidateSpxBytecode())
    {
        return error("Error validating the bytecode");
    }

    //=============================================================================================================
    //=============================================================================================================
    // Deal with base and overrides function

    //BEFORE checking for overrides, we first deal with base function calls
    if (!UpdateFunctionCallsHavingUnresolvedBaseAccessor())
    {
        return error("Updating function calls to base target failed");
    }

    //Update the list of overriding methods
    if (!UpdateOverridenFunctionMap(listNewShaders))
    {
        return error("Updating overriding functions failed");
    }

    //target function to the overring functions
    if (!UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions())
    {
        return error("Remapping overriding functions failed");
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

/*
bool SpxStreamRemapper::MergeAllNewShadersFromBytecode(const SpxBytecode& bytecode, vector<ShaderClassData*>& listShadersMerged)
{
    listShadersMerged.clear();
    int maxCountIds = bound();

    //=============================================================================================================
    //Init bytecode to merge
    SpxStreamRemapper bytecodeToMerge;
    if (!bytecodeToMerge.SetBytecode(bytecode)) return false;
    bool res = bytecodeToMerge.BuildAllMaps();

    if (!res) {
        bytecodeToMerge.copyMessagesTo(errorMessages);
        return error(string("Error building XKSL shaders data map from bytecode: ") + bytecode.GetName());
    }

    //=============================================================================================================
    //=============================================================================================================
    //Get the list of new shaders we want to merge (shaders not already in the destination bytecode)    
    vector<ShaderClassData*> listShadersToMerge;
    for (auto itsh = bytecodeToMerge.vecAllShaders.begin(); itsh != bytecodeToMerge.vecAllShaders.end(); itsh++)
    {
        ShaderClassData* shaderToMerge = *itsh;
        if (!GetShaderByName(shaderToMerge->GetName()))
        {
            listShadersToMerge.push_back(shaderToMerge);
        }
    }

    if (errorMessages.size() > 0) return false;
    if (listShadersToMerge.size() == 0) return true;  //no new shaders to merge

    if (!MergeShadersIntoBytecode(bytecodeToMerge, listShadersToMerge, ""))
    {
        return false;
    }

    //=============================================================================================================
    //retrieve the merged shaders from the destination bytecode
    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is];
        ShaderClassData* shaderMerged = GetShaderByName(shaderToMerge->GetName());
        if (shaderMerged == nullptr) return error(string("Cannot retrieve the shader: ") + shaderToMerge->GetName());
        listShadersMerged.push_back(shaderMerged);
    }

    return true;
}
*/

bool SpxStreamRemapper::MergeShadersIntoBytecode(SpxStreamRemapper& bytecodeToMerge, const vector<ShaderClassData*>& listShadersToMerge, string namesPrefixToAdd)
{
    //if (&bytecodeToMerge == this) {
    //    return error("Cannot merge a bytecode into its own code");
    //}

    //=============================================================================================================
    //Init destination bytecode hashmap (to compare types and consts hashmap id)
    unordered_map<uint32_t, pairIdPos> destinationBytecodeTypeHashMap;
    if (!this->BuildTypesAndConstsHashmap(destinationBytecodeTypeHashMap)) {
        return error("Merge shaders. Error building type and const hashmap");
    }

    //Init the positions of all objects from the bytecode to merge
    //if (!bytecodeToMerge.UpdateAllObjectsPositionInTheBytecode()) {
    //    return error("Error updating the position for all objects");
    //}

    //=============================================================================================================
    //=============================================================================================================
    //Merge all the shaders
    int newId = bound();

    spirvbin_t vecNamesAndDecorateToMerge;
    spirvbin_t vecXkslDecoratesToMerge;
    spirvbin_t vecTypesConstsAndVariablesToMerge;
    spirvbin_t vecFunctionsToMerge;
    spirvbin_t vecHeaderPropertiesToMerge;

    vector<spv::Id> finalRemapTable;
    finalRemapTable.resize(bytecodeToMerge.bound(), unused);
    //keep the list of merged Ids so that we can look for their name or decorate
    vector<bool> listAllNewIdMerged;
    vector<bool> listIdsWhereToAddNamePrefix;
    listAllNewIdMerged.resize(bytecodeToMerge.bound(), false);
    listIdsWhereToAddNamePrefix.resize(bytecodeToMerge.bound(), false);
    vector<pair<spv::Id, spv::Id>> listOverridenFunctionMergedToBeRemappedWithMergedFunctions;

    for (unsigned int is=0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is];
        if (shaderToMerge->bytecodeSource != &bytecodeToMerge)
        {
            error(string("Shader \"") + shaderToMerge->GetName() + string("\" does not belong to the source to merge"));
            return false;
        }

        shaderToMerge->tmpClonedShader = nullptr;

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
            if (finalRemapTable[type->GetId()] != unused) error(string("id: ") + to_string(type->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[pointerToType->GetId()] != unused) error(string("id: ") + to_string(pointerToType->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[variable->GetId()] != unused) error(string("id: ") + to_string(variable->GetId()) + string(" has already been remapped"));
#endif

            finalRemapTable[type->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, type->GetBytecodeStartPosition());
            finalRemapTable[pointerToType->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, pointerToType->GetBytecodeStartPosition());
            finalRemapTable[variable->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, variable->GetBytecodeStartPosition());
        }

        //Add the shader type declaration
        {
            const spv::Id resultId = shaderToMerge->GetId();

#ifdef XKSLANG_DEBUG_MODE
            if (finalRemapTable[resultId] != unused) error(string("id: ") + to_string(resultId) + string(" has already been remapped"));
#endif

            finalRemapTable[resultId] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, shaderToMerge->GetBytecodeStartPosition());
        }

        //=============================================================================================================
        //=============================================================================================================
        //add all functions' instructions declared by the shader
        for (unsigned int t = 0; t < shaderToMerge->functionsList.size(); ++t)
        {
            FunctionInstruction* functionToMerge = shaderToMerge->functionsList[t];
            //finalRemapTable[functionToMerge->id] = newId++; //done below

            //If the function is already overriden by another function: we'll update the link in the cloned functions as well
            if (functionToMerge->GetOverridingFunction() != nullptr)
            {
                listOverridenFunctionMergedToBeRemappedWithMergedFunctions.push_back(pair<spv::Id, spv::Id>(functionToMerge->GetId(), functionToMerge->GetOverridingFunction()->GetId()));
            }
            
            //For each instructions within the functions bytecode: Remap their results IDs
            bytecodeToMerge.process(
                [&](spv::Op opCode, unsigned start)
                {
                    unsigned word = start + 1;

                    // Read type and result ID from instruction desc table
                    if (spv::InstructionDesc[opCode].hasType()) {
                        word++;  //spv::Id typeId = bytecodeToMerge.asId(word++);
                    }

                    if (spv::InstructionDesc[opCode].hasResult()) {
                        spv::Id resultId = bytecodeToMerge.asId(word++);
#ifdef XKSLANG_DEBUG_MODE
                        if (finalRemapTable[resultId] != unused) error(string("id: ") + to_string(resultId) + string(" has already been remapped"));
#endif
                        finalRemapTable[resultId] = newId++;
                    }

                    return true;
                },
                spx_op_fn_nop,
                functionToMerge->GetBytecodeStartPosition(), functionToMerge->GetBytecodeEndPosition()
            );

            //Copy all bytecode instructions from the function
            bytecodeToMerge.CopyInstructionToVector(vecFunctionsToMerge.spv, functionToMerge->GetBytecodeStartPosition(), functionToMerge->GetBytecodeEndPosition());
        }
    } //end shaderToMerge loop

    {
        int len = finalRemapTable.size();
        //update listAllNewIdMerged table (this table defines the name and decorate to fetch and merge)
        for (int i = 0; i < len; ++i)
        {
            if (finalRemapTable[i] != unused) listAllNewIdMerged[i] = true;
        }
        //this list defines ids for which we update the names and labels with a prefix
        if (namesPrefixToAdd.size() > 0)
        {
            for (int i = 0; i < len; ++i)
            {
                if (finalRemapTable[i] != unused) listIdsWhereToAddNamePrefix[i] = true;
            }
        }
    }

    //Populate vecNewShadersDecorationPossesingIds with some decorate instructions which might contains unmapped IDs
    spirvbin_t vecXkslDecorationsPossesingIds;
    bytecodeToMerge.process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode)
            {
                case spv::OpShaderInheritance:
                case spv::OpShaderComposition:
                case spv::OpShaderArrayComposition:
                {
                    const spv::Id id = bytecodeToMerge.asId(start + 1);
                    if (listAllNewIdMerged[id])
                    {
                        bytecodeToMerge.CopyInstructionToVector(vecXkslDecorationsPossesingIds.spv, start);
                    }
                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    //=============================================================================================================
    //=============================================================================================================
    //Check for all unmapped Ids called within the shader types/consts instructions that we have to merge
    spirvbin_t bytecodeWithExtraTypesToMerge;
    {
        //Init by checking all types instructions for unmapped IDs
        spirvbin_t bytecodeToCheckForUnmappedIds;
        bytecodeToCheckForUnmappedIds.spv.insert(bytecodeToCheckForUnmappedIds.spv.end(), vecTypesConstsAndVariablesToMerge.spv.begin(), vecTypesConstsAndVariablesToMerge.spv.end());
        bytecodeToCheckForUnmappedIds.spv.insert(bytecodeToCheckForUnmappedIds.spv.end(), vecFunctionsToMerge.spv.begin(), vecFunctionsToMerge.spv.end());
        bytecodeToCheckForUnmappedIds.spv.insert(bytecodeToCheckForUnmappedIds.spv.end(), vecXkslDecorationsPossesingIds.spv.begin(), vecXkslDecorationsPossesingIds.spv.end());
        vector<pairIdPos> listUnmappedIdsToProcess;  //unmapped ids and their pos in the bytecode to merge

        spv::Op opCode;
        unsigned int wordCount;
        vector<spv::Id> listIds;
        spv::Id typeId, resultId;
        unsigned int listIdsLen;
        unsigned int pos = 0;
        const unsigned int end = bytecodeToCheckForUnmappedIds.spv.size();
        while (pos < end)
        {
            listIds.clear();
            if (!bytecodeToCheckForUnmappedIds.parseInstruction(pos, opCode, wordCount, typeId, resultId, listIds))
                return error("Merge shaders. Error parsing bytecodeToCheckForUnmappedIds");

            if (typeId != unused) listIds.push_back(typeId);
            listIdsLen = listIds.size();
            for (unsigned int i = 0; i < listIdsLen; ++i)
            {
                const spv::Id id = listIds[i];
                if (finalRemapTable[id] == unused)
                {
                    listUnmappedIdsToProcess.push_back(pairIdPos(id, -1));
                }
            }

            pos += wordCount;
        }

        //TypeData* typeToMerge;
        //ConstData* constToMerge;
        //VariableData* variableToAccess;
        while (listUnmappedIdsToProcess.size() > 0)
        {
            pairIdPos& unmappedIdPos = listUnmappedIdsToProcess.back();
            const spv::Id unmappedId = unmappedIdPos.first;
            if (finalRemapTable[unmappedId] != unused)
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

                        if (idOfSameTypeFromDestinationBytecode == unused)
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
                        bytecodeToMerge.CopyInstructionToVector(vecHeaderPropertiesToMerge.spv, objectFromUnmappedId->GetBytecodeStartPosition());
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
                    bytecodeToCheckForUnmappedIds.spv.clear();
                    bytecodeToMerge.CopyInstructionToVector(bytecodeToCheckForUnmappedIds.spv, objectFromUnmappedId->GetBytecodeStartPosition());
                    listIds.clear();
                    if (!bytecodeToCheckForUnmappedIds.parseInstruction(0, opCode, wordCount, typeId, resultId, listIds))
                        return error("Error parsing bytecodeToCheckForUnmappedIds");

                    if (typeId != unused) listIds.push_back(typeId);
                    listIdsLen = listIds.size();
                    bool canAddTheInstruction = true;
                    for (unsigned int i = 0; i < listIdsLen; ++i)
                    {
                        const spv::Id anotherId = listIds[i];
#ifdef XKSLANG_DEBUG_MODE
                        if (anotherId == unmappedId) return error(string("anotherId == unmappedId: ") + to_string(anotherId) + string(". This should be impossible (bytecode is invalid)"));
#endif
                        if (finalRemapTable[anotherId] == unused)
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
                        bytecodeToMerge.CopyInstructionToVector(bytecodeWithExtraTypesToMerge.spv, objectFromUnmappedId->GetBytecodeStartPosition());

                        listAllNewIdMerged[unmappedId] = true;
                    }
                }
            }
        }  //end while (listUnmappedIdsToProcess.size() > 0)
    }  //end block

    //Add the extra types we merged at the beginning of our vec of type/const/variable
    if (bytecodeWithExtraTypesToMerge.spv.size() > 0)
    {
        vecTypesConstsAndVariablesToMerge.spv.insert(vecTypesConstsAndVariablesToMerge.spv.begin(), bytecodeWithExtraTypesToMerge.spv.begin(), bytecodeWithExtraTypesToMerge.spv.end());       
    }

    //=============================================================================================================
    //=============================================================================================================
    // get all name, decoration and XKSL data for the new IDs we need to merge
    bytecodeToMerge.process(
        [&](spv::Op opCode, unsigned start)
        {
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
                            string strUpdatedName(namesPrefixToAdd + bytecodeToMerge.literalString(start + 2));
                            spv::Instruction nameInstr(opCode);
                            nameInstr.addIdOperand(id);
                            nameInstr.addStringOperand(strUpdatedName.c_str());
                            nameInstr.dump(vecNamesAndDecorateToMerge.spv);
                        }
                        else
                        {
                            bytecodeToMerge.CopyInstructionToVector(vecNamesAndDecorateToMerge.spv, start);
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
                        bytecodeToMerge.CopyInstructionToVector(vecNamesAndDecorateToMerge.spv, start);
                    }
                    break;
                }
                
                case spv::OpDeclarationName:
                {
                    //We update the declaration name only for shader classes
                    const spv::Id id = bytecodeToMerge.asId(start + 1);
                    if (listAllNewIdMerged[id])
                    {
                        bool addPrefix = false;
                        if (listIdsWhereToAddNamePrefix[id])
                        {
                            ShaderClassData* shader = bytecodeToMerge.GetShaderById(id);
                            if (shader != nullptr) addPrefix = true;
                        }

                        if (addPrefix)
                        {
                            //disassemble and reassemble the instruction with a name updated with the prefix
                            string strUpdatedName(namesPrefixToAdd + bytecodeToMerge.literalString(start + 2));
                            spv::Instruction nameInstr(opCode);
                            nameInstr.addIdOperand(id);
                            nameInstr.addStringOperand(strUpdatedName.c_str());
                            nameInstr.dump(vecXkslDecoratesToMerge.spv);
                        }
                        else
                        {
                            bytecodeToMerge.CopyInstructionToVector(vecXkslDecoratesToMerge.spv, start);
                        }
                    }
                    break;
                }

                case spv::OpBelongsToShader:
                case spv::OpShaderInheritance:
                case spv::OpShaderComposition:
                case spv::OpShaderArrayComposition:
                case spv::OpMethodProperties:
                {
                    const spv::Id id = bytecodeToMerge.asId(start + 1);
                    if (listAllNewIdMerged[id])
                    {
                        bytecodeToMerge.CopyInstructionToVector(vecXkslDecoratesToMerge.spv, start);
                    }
                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    //=============================================================================================================
    //=============================================================================================================
    //remap IDs for all mergable types / variables / consts / functions / extImport
    if (vecHeaderPropertiesToMerge.spv.size() > 0)
    {
        if (!vecHeaderPropertiesToMerge.remapAllIds(0, vecHeaderPropertiesToMerge.spv.size(), finalRemapTable))
            return error("remapAllIds failed on vecHeaderPropertiesToMerge");
    }
    if (!vecTypesConstsAndVariablesToMerge.remapAllIds(0, vecTypesConstsAndVariablesToMerge.spv.size(), finalRemapTable))
        return error("remapAllIds failed on vecTypesConstsAndVariablesToMerge");
    if (!vecXkslDecoratesToMerge.remapAllIds(0, vecXkslDecoratesToMerge.spv.size(), finalRemapTable))
        return error("remapAllIds failed on vecXkslDecoratesToMerge");
    if (!vecNamesAndDecorateToMerge.remapAllIds(0, vecNamesAndDecorateToMerge.spv.size(), finalRemapTable))
        return error("remapAllIds failed on vecNamesAndDecorateToMerge");
    if (!vecFunctionsToMerge.remapAllIds(0, vecFunctionsToMerge.spv.size(), finalRemapTable))
        return error("remapAllIds failed on vecFunctionsToMerge");
    //vecFunctionsToMerge.processOnFullBytecode(
    //    spx_inst_fn_nop,
    //    [&](spv::Id& id)
    //    {
    //        spv::Id newId = finalRemapTable[id];
    //        if (newId == unused) error(string("Invalid remapper Id: ") + to_string(id));
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
    unsigned int posToInsertNewFunctions = spv.size();
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
                case spv::OpShaderComposition:
                case spv::OpShaderArrayComposition:
                case spv::OpMethodProperties:
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
    spv.insert(spv.begin() + posToInsertNewFunctions, vecFunctionsToMerge.spv.begin(), vecFunctionsToMerge.spv.end());
    //Merge types and variables (we need to merge new types AFTER all previous types)
    spv.insert(spv.begin() + posToInsertNewTypesAndConsts, vecTypesConstsAndVariablesToMerge.spv.begin(), vecTypesConstsAndVariablesToMerge.spv.end());
    //Merge names and decorates
    spv.insert(spv.begin() + posToInsertNewXkslDecorates, vecXkslDecoratesToMerge.spv.begin(), vecXkslDecoratesToMerge.spv.end());
    spv.insert(spv.begin() + posToInsertNewNames, vecNamesAndDecorateToMerge.spv.begin(), vecNamesAndDecorateToMerge.spv.end());
    //merge ext import
    spv.insert(spv.begin() + posToInsertNewHeaderProrerties, vecHeaderPropertiesToMerge.spv.begin(), vecHeaderPropertiesToMerge.spv.end());

    //destination bytecode has been updated: reupdate all maps
    UpdateAllMaps();

    //Set reference between shaders merged and the clone shaders from destination bytecode
    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is];
        spv::Id clonedShaderId = finalRemapTable[shaderToMerge->GetId()];
        ShaderClassData* clonedShader = this->GetShaderById(clonedShaderId);
        if (clonedShader == nullptr) return error(string("Cannot retrieve the merged shader: ") + to_string(clonedShaderId));
        shaderToMerge->tmpClonedShader = clonedShader;
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

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::SetBytecode(const SpxBytecode& bytecode)
{
    const vector<uint32_t>& spx = bytecode.getBytecodeStream();
    spv.clear();
    spv.insert(spv.end(), spx.begin(), spx.end());

    if (!ValidateSpxBytecode())
        return error(string("Invalid bytecode: ") + bytecode.GetName());

    return true;
}

bool SpxStreamRemapper::ValidateHeader()
{
    if (spv.size() < header_size) {
        return error("file too short");
    }

    if (magic() != spv::MagicNumber)
        error("bad magic number");

    // field 1 = version
    // field 2 = generator magic
    // field 3 = result <id> bound

    if (schemaNum() != 0)
        error("bad schema, must be 0");

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::ValidateSpxBytecode()
{
    if (!ValidateHeader()) return error("Failed to validate the header");

#ifdef XKSLANG_DEBUG_MODE
    //Debug sanity check: make sure that 2 shaders or 2 variables does not share the same name
    unordered_map<string, int> shaderVariablesDeclarationName;
    unordered_map<string, int> shadersFunctionsDeclarationName;
    unordered_map<string, int> shadersDeclarationName;

    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
    {
        ShaderClassData* aShader = *itsh;
        if (shadersDeclarationName.find(aShader->GetName()) != shadersDeclarationName.end())
                return error(string("A shader already exists with the name: ") + aShader->GetName());
        shadersDeclarationName[aShader->GetName()] = 1;

        //2 different shaders cannot share an identical variable name
        for (auto itt = aShader->shaderTypesList.begin(); itt != aShader->shaderTypesList.end(); itt++)
        {
            ShaderTypeData* type = *itt;
            if (shaderVariablesDeclarationName.find(type->variable->GetName()) != shaderVariablesDeclarationName.end())
                return error(string("A variable already exists with the name: ") + type->variable->GetName());
            shaderVariablesDeclarationName[type->variable->GetName()] = 1;
        }
    }
#endif

    return true;
}

bool SpxStreamRemapper::AddComposition(const string& shaderName, const string& variableName, SpxStreamRemapper* source, vector<string>& msgs)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    //===================================================================================================================
    //===================================================================================================================
    //Find the shader target
    ShaderClassData* shaderCompositionTarget = this->GetShaderByName(shaderName);
    if (shaderCompositionTarget == nullptr) return error(string("No shader exists in destination bytecode with the name: ") + shaderName);

    //find the shader's composition target
    ShaderComposition* compositionTarget = shaderCompositionTarget->GetShaderCompositionByName(variableName);
    if (compositionTarget == nullptr) return error(string("No composition exists in shader \"") + shaderName + string("\" with the name: ") + variableName);
    if (compositionTarget->status == ShaderComposition::ShaderCompositionStatusEnum::Resolved) return error(string("The composition has already been resolved"));
    int compositionTargetId = compositionTarget->compositionShaderId;

    ShaderClassData* shaderTypeToInstantiate = compositionTarget->shaderType;
    if (shaderTypeToInstantiate == nullptr) return error("The composition does not define any shader type");

    ShaderClassData* mainShaderTypeMergedToMergeFromSource = source->GetShaderByName(shaderTypeToInstantiate->GetName());
    if (mainShaderTypeMergedToMergeFromSource == nullptr) return error(string("No shader exists in the source bytecode with the name: ") + shaderTypeToInstantiate->GetName());

    //===================================================================================================================
    //===================================================================================================================
    //Get the list of all shaders belonging to shaderToInstantiate's family tree
    for (auto itsh = source->vecAllShaders.begin(); itsh != source->vecAllShaders.end(); itsh++) (*itsh)->flag1 = 0;

    vector<ShaderClassData*> listAllShadersToInstantiate;
    vector<ShaderClassData*> listShadersToInvestigate;
    listShadersToInvestigate.push_back(mainShaderTypeMergedToMergeFromSource);
    while (listShadersToInvestigate.size() > 0)
    {
        ShaderClassData* aShaderToInstantiate = listShadersToInvestigate.back();
        listShadersToInvestigate.pop_back();
        
        vector<ShaderClassData*> shaderFamilyTree;
        source->GetShaderFamilyTree(aShaderToInstantiate, shaderFamilyTree);

        for (auto its = shaderFamilyTree.begin(); its != shaderFamilyTree.end(); its++)
        {
            ShaderClassData* aShaderFromFamily = *its;
            if (aShaderFromFamily->flag1 == 1)
                return error(string("A shader is being investigated after having already been treated"));  //safety check
            aShaderFromFamily->flag1 = 1;

            listAllShadersToInstantiate.push_back(aShaderFromFamily);

            //For all shaders to instantiate: we recursively add all shaders from their resolved compositions, and their full family tree
            int countCompositions = aShaderFromFamily->GetCountShaderComposition();
            for (int ic = 0; ic < countCompositions; ++ic)
            {
                ShaderComposition& aShaderComposition = aShaderFromFamily->compositionsList[ic];
                if (aShaderComposition.status == ShaderComposition::ShaderCompositionStatusEnum::Unresolved)
                    return error(string("A shader to instantiate has some unresolved composition variables. Shader name:") + aShaderFromFamily->GetName());

                listShadersToInvestigate.push_back(aShaderComposition.shaderType);
            }
        }
    }

    if (listAllShadersToInstantiate.size() == 0)
        return error(string("Failed to find some shaders to instantiate for the family of shader:") + mainShaderTypeMergedToMergeFromSource->GetName());

    //===================================================================================================================
    //===================================================================================================================
    //prefix to rename the shader, its variables and functions (useful in debug to check what's going on into the byteconde)
    //string namePrefix = string("c") + compositionToInstantiate->compositionShaderOwner->GetName() + compositionToInstantiate->variableName + string("_");
    string namePrefix = string("cS") + to_string(compositionTarget->compositionShaderOwner->GetId()) + string("C") + to_string(compositionTargetId) + string("_");

    //Merge (duplicate) all shaders targeted by by the composition into the current bytecode
    if (!MergeShadersIntoBytecode(*source, listAllShadersToInstantiate, namePrefix))
    {
        error(string("failed to clone the shader for the composition: ") + namePrefix);
        return false;
    }
    ShaderClassData* mainShaderTypeMerged = mainShaderTypeMergedToMergeFromSource->tmpClonedShader;
    if (mainShaderTypeMerged == nullptr)
        return error(string("Merge shader function is expected to return a reference on the merged shader"));

    //===================================================================================================================
    //Map the list of targeted shaders with the cloned shader (so that we can update the links later on)
    unordered_map<spv::Id, spv::Id> mapCompositionShaderTargetedWithClonedShader;
    for (auto its = listAllShadersToInstantiate.begin(); its != listAllShadersToInstantiate.end(); its++)
    {
        ShaderClassData* shaderCloned = *its;
        ShaderClassData* clonedShader = shaderCloned->tmpClonedShader;
        mapCompositionShaderTargetedWithClonedShader[shaderCloned->GetId()] = clonedShader->GetId();
    }

    //===================================================================================================================
    //===================================================================================================================
    // - update all OpFunctionCallThroughCompositionVariable instructions calling through the composition that we're instancing
    // - update our composition data
    vector<range_t> vecStripRanges;
    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode)
            {
                case spv::OpShaderComposition:
                {
                    //set shaderType Id to the merged type id
                    spv::Id shaderId = asId(start + 1);
                    int compositionId = asId(start + 2);

                    if (shaderId == shaderCompositionTarget->GetId() && compositionId == compositionTargetId)
                    {
                        //===================================================================================================================
                        //update the composition type referenced by the new merged shader
                        compositionTarget->status = ShaderComposition::ShaderCompositionStatusEnum::Resolved;
                        compositionTarget->shaderType = mainShaderTypeMerged;
                        spv::Id mergedShaderTypeId = mainShaderTypeMerged->GetId();
                        
                        setId(start + 3, mergedShaderTypeId);
                        setId(start + 4, 1);  //set status to resolved
                    }
                    break;
                }

                case spv::OpShaderArrayComposition:
                    return error("not implemented yet");
                    break;

                case spv::OpFunctionCallThroughCompositionVariable:
                {
                    spv::Id shaderId = asId(start + 4);
                    int compositionId = asId(start + 5);
    
                    if (shaderId == shaderCompositionTarget->GetId() && compositionId == compositionTargetId)
                    {
                        //Get function and shader reference in the destination bytecode
                        spv::Id originalFunctionId = asId(start + 3);
                        FunctionInstruction* functionToReplace = GetFunctionById(originalFunctionId);
                        if (functionToReplace == nullptr){
                            error(string("OpFunctionCallThroughCompositionVariable: targeted Id is not a known function. Id: ") + to_string(originalFunctionId));
                            return true;
                        }
                        ShaderClassData* shaderOwningTheFunction = functionToReplace->GetShaderOwner();
                        if (shaderOwningTheFunction == nullptr) {
                            error(string("OpFunctionCallThroughCompositionVariable: function to replace has no shader owner: ") + functionToReplace->GetName());
                            return true;
                        }
                        const string& shaderName = shaderOwningTheFunction->GetName();

                        //Find this shader in the list of shader we cloned from the source bytecode
                        ShaderClassData* shaderClonedOwningTheFunction = nullptr;
                        for (int i = 0; i < listAllShadersToInstantiate.size(); ++i)
                        {
                            if (listAllShadersToInstantiate[i]->GetName() == shaderName) {
                                shaderClonedOwningTheFunction = listAllShadersToInstantiate[i];
                                break;
                            }
                        }

                        if (shaderClonedOwningTheFunction == nullptr) {
                            error(string("We cannot find back the composition shader type from the list of shader cloned. Name: ") + shaderName);
                            return true;
                        }

                        //look for the cloned shader
                        auto itmap = mapCompositionShaderTargetedWithClonedShader.find(shaderClonedOwningTheFunction->GetId());
                        if (itmap == mapCompositionShaderTargetedWithClonedShader.end())
                        {
                            error(string("OpFunctionCallThroughCompositionVariable: failed to find the shader cloned named: ") + shaderName);
                            return true;
                        }

                        spv::Id clonedShaderId = itmap->second;
                        ShaderClassData* clonedShader = GetShaderById(clonedShaderId);
                        if (clonedShader == nullptr) {
                            error(string("OpFunctionCallThroughCompositionVariable: cloned shader Id cannotbe retrieve in the destination bytecode. Id: ") + to_string(clonedShaderId));
                            return true;
                        }
    
                        FunctionInstruction* functionTarget = clonedShader->GetFunctionByName(functionToReplace->GetName());
                        if (functionTarget == nullptr) {
                            error(string("OpFunctionCallThroughCompositionVariable: cannot retrieve the function in the cloned shader. Function name: ") + functionToReplace->GetName());
                            return true;
                        }

                        //If the function is overriden by another, change the target
                        if (functionTarget->GetOverridingFunction() != nullptr) functionTarget = functionTarget->GetOverridingFunction();
    
                        int wordCount = asWordCount(start);
                        vecStripRanges.push_back(range_t(start + 4, start + 5 + 1));   //will remove composition variable ids from the bytecode
                        setOpAndWordCount(start, spv::OpFunctionCall, wordCount - 2);  //change instruction OpFunctionCallThroughCompositionVariable to OpFunctionCall
                        setId(start + 3, functionTarget->GetId());                     //replace the function called id
                    }
                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) {
        return error("failed to retarget the functions called by the compositions");
    }

    //remove strip bytecode
    stripBytecode(vecStripRanges);

    //Update all maps
    //TODO: We're not removing any instructions or objects (only reducing instruction for FunctionCallThroughCompositionVar)
    //So we could optimize this by simply updating the start-end position for all objects
    UpdateAllMaps();

    if (compositionTarget->status != ShaderComposition::ShaderCompositionStatusEnum::Resolved)
        return error("The composition has not been resolved");

    if (errorMessages.size() > 0) return false;
    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}

//Mixin is finalized: no more updates will be brought to the mixin bytecode after
bool SpxStreamRemapper::CompileMixinForStages(vector<XkslMixerOutputStage>& outputStages)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }

    if (outputStages.size() == 0) return error("no output stages defined");
    if (!ValidateHeader()) return error("Failed to validate the header");

    //===================================================================================================================
    //===================================================================================================================
    //For each output stages, we search the entryPoint function in the bytecode
    vector<FunctionInstruction*> listEntryPointFunction;
    for (int i = 0; i<outputStages.size(); ++i)
    {
        FunctionInstruction* entryFunction = GetFunctionForEntryPoint(outputStages[i].entryPointName);
        if (entryFunction == nullptr) error(string("Entry point not found: ") + outputStages[i].entryPointName);
        listEntryPointFunction.push_back(entryFunction);
    }
    if (errorMessages.size() > 0) return false;

    status = SpxRemapperStatusEnum::MixinFinalized;

    //===================================================================================================================
    //===================================================================================================================
    //Find all unused shader classes from the bytecode, depending on the outputStages entryPoint functions
    {
        //Set all shaders and functions flag to 0
        for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) {
            ShaderClassData* shader = *itsh;
            shader->flag1 = 0;
            for (auto itsf = shader->functionsList.begin(); itsf != shader->functionsList.end(); itsf++) {
                FunctionInstruction* aFunction = *itsf;
                aFunction->flag1 = 0;
            }
        }

        //====================================================================
        //Analyse all functions called, starting from all entryPoint functions
        vector<FunctionInstruction*> vectorFunctionsCalled;
        vector<FunctionInstruction*> vectorAllFunctionsCalled;
        vector<ShaderClassData*> vectorShadersOwningAllFunctionsCalled;
        vectorFunctionsCalled.insert(vectorFunctionsCalled.end(), listEntryPointFunction.begin(), listEntryPointFunction.end());
        while (vectorFunctionsCalled.size() > 0)
        {
            FunctionInstruction* aFunctionCalled = vectorFunctionsCalled.back();
            vectorFunctionsCalled.pop_back();
            if (aFunctionCalled->flag1 == 1) continue; //we already processed this function
            vectorAllFunctionsCalled.push_back(aFunctionCalled);

            //Flag the function and the shader owning it
            aFunctionCalled->flag1 = 1;
            if (aFunctionCalled->shaderOwner->flag1 == 0)
                vectorShadersOwningAllFunctionsCalled.push_back(aFunctionCalled->shaderOwner);
            aFunctionCalled->shaderOwner->flag1 = 1;

            //Analyse the function's bytecode, to add all new functions called
            unsigned int start = aFunctionCalled->bytecodeStartPosition;
            const unsigned int end = aFunctionCalled->bytecodeEndPosition;
            while (start < end)
            {
                unsigned int wordCount = asWordCount(start);
                spv::Op opCode = asOpCode(start);

                switch (opCode)
                {
                    case spv::OpFunctionCall:
                    case spv::OpFunctionCallBaseResolved:
                    {
                        spv::Id functionCalledId = asId(start + 3);
                        FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                        if (anotherFunctionCalled->flag1 == 0) vectorFunctionsCalled.push_back(anotherFunctionCalled); //we'll ananlyse the function later
                        break;
                    }

                    case spv::OpFunctionCallBaseUnresolved:
                    {
                        error(string("An unresolved function base call has been found in function: ") + aFunctionCalled->GetName());
                        break;
                    }

                    case spv::OpFunctionCallThroughCompositionVariable:
                    {
                        error(string("An unresolved function call through composition has been found in function: ") + aFunctionCalled->GetName());
                        break;
                    }
                }
                start += wordCount;
            }
        }
        if (errorMessages.size() > 0) return false;

        //====================================================================
        //For all shader flagged, flag their whole family (compositions and parents) as well
        vector<ShaderClassData*> vectorAllShadersInvolved;
        while (vectorShadersOwningAllFunctionsCalled.size() > 0)
        {
            ShaderClassData* aShaderInvolved = vectorShadersOwningAllFunctionsCalled.back();
            vectorShadersOwningAllFunctionsCalled.pop_back();
            if (aShaderInvolved->flag1 == 2) continue;
            
            aShaderInvolved->flag1 = 2;
            vectorAllShadersInvolved.push_back(aShaderInvolved);

            //add the shader parents
            for (auto itsh = aShaderInvolved->parentsList.begin(); itsh != aShaderInvolved->parentsList.end(); itsh++)
            {
                ShaderClassData* aShaderParentInvolved = *itsh;
                if (aShaderParentInvolved->flag1 != 2) vectorShadersOwningAllFunctionsCalled.push_back(aShaderParentInvolved);
            }
            
            //add the compositions' shader target, check that they've been resolved
            for (auto itc = aShaderInvolved->compositionsList.begin(); itc != aShaderInvolved->compositionsList.end(); itc++)
            {
                const ShaderComposition& aComposition = *itc;
                if (aComposition.status == ShaderComposition::ShaderCompositionStatusEnum::Unresolved)
                {
                    error(string("Composition \"") + aComposition.GetVariableName() + string("\" from shader \"") + aShaderInvolved->GetName() + string("\" has not been set"));
                }
                else
                {
                    ShaderClassData* aShaderCompositionInvolved = itc->shaderType;
                    if (aShaderCompositionInvolved->flag1 != 2) vectorShadersOwningAllFunctionsCalled.push_back(aShaderCompositionInvolved);
                }
            }
        }
        if (errorMessages.size() > 0) return false;
    }

    //===================================================================================================================
    //===================================================================================================================
    //Remove unused shaders
    {
        //Get the list of all unused (unflagged) shaders
        vector<ShaderClassData*> listUnusedShaders;
        for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) {
            ShaderClassData* shader = *itsh;
            if (shader->flag1 != 2) listUnusedShaders.push_back(shader);
        }

        //remove all unused shaders from the bytecode and the data structure
        vector<range_t> vecStripRanges;
        for (auto itsh = listUnusedShaders.begin(); itsh != listUnusedShaders.end(); itsh++) {
            ShaderClassData* unusedShader = *itsh;
            if (!RemoveShaderAndAllData(unusedShader, vecStripRanges))
            {
                return error(string("Failed to remove the unused shader: ") + unusedShader->GetName());
            }
        }

        //warning: objects bytecode position is not correct anymore after this 
        stripBytecode(vecStripRanges);
    }
    
    //===================================================================================================================
    //===================================================================================================================
    //Convert SPIRX extensions to SPIRV
    {
        vector<range_t> vecStripRanges;
        process(
            [&](spv::Op opCode, unsigned start)
            {
                switch (opCode) {
                    case spv::OpFunctionCallBaseResolved:
                    {
                        //change OpFunctionCallBaseResolved to OpFunctionCall
                        setOpCode(start, spv::OpFunctionCall);
                        break;
                    }
                    case spv::OpFunctionCallThroughCompositionVariable:
                    {
                        error(string("Found unresolved OpFunctionCallThroughCompositionVariable. pos=") + to_string(start));
                        break;
                    }
                    case spv::OpFunctionCallBaseUnresolved:
                    {
                        error(string("A function base call has an unresolved state. Pos=") + to_string(start));
                        break;
                    }
                    case spv::OpBelongsToShader:
                    case spv::OpDeclarationName:
                    case spv::OpTypeXlslShaderClass:
                    case spv::OpShaderInheritance:
                    case spv::OpShaderComposition:
                    case spv::OpShaderArrayComposition:
                    case spv::OpMethodProperties:
                    {
                        stripInst(vecStripRanges, start);
                        break;
                    }
                }
                return true;
            },
            spx_op_fn_nop
        );
        if (errorMessages.size() > 0) return false;

        //warning: objects bytecode position is not correct anymore after this 
        stripBytecode(vecStripRanges);
    }

    //===================================================================================================================
    //===================================================================================================================
    // Generate the SPIRV bytecode for all stages
    for (int i = 0; i<outputStages.size(); ++i)
    {
        XkslMixerOutputStage& outputStage = outputStages[i];
        FunctionInstruction* entryFunction = listEntryPointFunction[i];
        bool success = this->GenerateSpvStageBytecode(outputStage.stage, outputStage.entryPointName, entryFunction, outputStage.resultingBytecode);
        if (!success)
        {
            error(string("Fail to generate SPV stage bytecode for the stage: ") + GetShadingStageLabel(outputStage.stage));
        }
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

//For every call to a function using base accessor (base.function()), we check if we need to redirect to another overriding method
bool SpxStreamRemapper::UpdateFunctionCallsHavingUnresolvedBaseAccessor()
{
    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode) {
                case spv::OpFunctionCallBaseUnresolved:
                {
                    setOpCode(start, spv::OpFunctionCallBaseResolved); // change OpFunctionCallBaseUnresolved to OpFunctionCallBaseResolved

                    spv::Id functionCalledId = asId(start + 3);

                    FunctionInstruction* functionCalled = GetFunctionById(functionCalledId);
                    if (functionCalled != nullptr)
                    {
                        if (functionCalled->GetOverridingFunction() != nullptr)
                        {
                            spv::Id overridingFunctionId = functionCalled->GetOverridingFunction()->GetResultId();
                            spv[start + 3] = overridingFunctionId;
                        }
                    }
                    else
                        error(string("OpFunctionCallBaseUnresolved: targeted Id is not a known function. Id: ") + to_string(functionCalledId));

                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
        );

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions()
{
    vector<FunctionInstruction*> vecFunctionIdBeingOverriden;
    vecFunctionIdBeingOverriden.resize(bound(), nullptr);
    bool anyOverridingFunction = false;
    for (auto itfn = vecAllShaderFunctions.begin(); itfn != vecAllShaderFunctions.end(); itfn++)
    {
        FunctionInstruction* function = *itfn;
        if (function->GetOverridingFunction() != nullptr)
        {
            vecFunctionIdBeingOverriden[function->GetResultId()] = function->GetOverridingFunction();
            anyOverridingFunction = true;
        }
    }

    if (!anyOverridingFunction) return true; //nothing to override

    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode) {
                case spv::OpFunctionCall:
                case spv::OpFunctionCallThroughCompositionVariable:
                {
                    spv::Id functionCalledId = asId(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                    if (functionCalledId < 0 || functionCalledId >= vecFunctionIdBeingOverriden.size()){
                        error(string("function call Id is out of bound. Id: ") + to_string(functionCalledId));
                        return true;
                    }
#endif
                    FunctionInstruction* overridingFunction = vecFunctionIdBeingOverriden[functionCalledId];
                    if (overridingFunction != nullptr)
                    {
                        spv::Id overridingFunctionId = overridingFunction->GetResultId();
                        spv[start + 3] = overridingFunctionId;
                    }

                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) return false;
    return true;
}

void SpxStreamRemapper::GetShaderChildrenList(ShaderClassData* shader, vector<ShaderClassData*>& children)
{
    children.clear();
    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
    {
        ShaderClassData* aShader = *itsh;
        if (aShader == shader) continue;

        if (aShader->HasParent(shader)) children.push_back(aShader);
    }
}

void SpxStreamRemapper::GetShaderFamilyTreeWithParentAndCompositionType(ShaderClassData* shaderFromFamily, vector<ShaderClassData*>& shaderFamilyTree)
{
    shaderFamilyTree.clear();

    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) (*itsh)->flag = 0;

    vector<ShaderClassData*> children;
    vector<ShaderClassData*> listShaderToValidate;
    listShaderToValidate.push_back(shaderFromFamily);
    while (listShaderToValidate.size() > 0)
    {
        ShaderClassData* aShader = listShaderToValidate.back();
        listShaderToValidate.pop_back();

        if (aShader->flag != 0) continue;  //the shader has already been added
        shaderFamilyTree.push_back(aShader);
        aShader->flag = 1;

        //Add all parents
        for (auto itsh = aShader->parentsList.begin(); itsh != aShader->parentsList.end(); itsh++)
            listShaderToValidate.push_back(*itsh);

        //Add all compositions
        for (auto itc = aShader->compositionsList.begin(); itc != aShader->compositionsList.end(); itc++)
            listShaderToValidate.push_back(itc->shaderType);
    }
}

void SpxStreamRemapper::GetShaderFamilyTree(ShaderClassData* shaderFromFamily, vector<ShaderClassData*>& shaderFamilyTree)
{
    shaderFamilyTree.clear();

    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) (*itsh)->flag = 0;

    vector<ShaderClassData*> children;
    vector<ShaderClassData*> listShaderToValidate;
    listShaderToValidate.push_back(shaderFromFamily);
    while (listShaderToValidate.size() > 0)
    {
        ShaderClassData* aShader = listShaderToValidate.back();
        listShaderToValidate.pop_back();

        if (aShader->flag != 0) continue;  //the shader has already been added
        shaderFamilyTree.push_back(aShader);
        aShader->flag = 1;

        //Add all parents
        for (auto itsh = aShader->parentsList.begin(); itsh != aShader->parentsList.end(); itsh++)
            listShaderToValidate.push_back(*itsh);

        //Add all children
        GetShaderChildrenList(aShader, children);
        for (auto itsh = children.begin(); itsh != children.end(); itsh++)
            listShaderToValidate.push_back(*itsh);
    }
}

static bool shaderLevelSortFunction(SpxStreamRemapper::ShaderClassData* sa, SpxStreamRemapper::ShaderClassData* sb)
{
    return (sa->level < sb->level);
}

bool SpxStreamRemapper::UpdateOverridenFunctionMap(vector<ShaderClassData*>& listShadersMerged)
{
    if (listShadersMerged.size() == 0) return true;

    //we need to know the shader level before building the overriding map
    if (!ComputeShadersLevel())
        return error(string("Failed to build the shader level"));

    //sort the shaders according to their level
    sort(listShadersMerged.begin(), listShadersMerged.end(), shaderLevelSortFunction);

    for (auto itshaderMerged = listShadersMerged.begin(); itshaderMerged != listShadersMerged.end(); itshaderMerged++)
    {
        ShaderClassData* shaderWithOverridingFunctions = *itshaderMerged;
        vector<ShaderClassData*> shaderFamilyTree;
        vector<FunctionInstruction*> listOverridingFunctions;

        //get the list of all functions having the override attribute
        for (auto itOverridingFn = shaderWithOverridingFunctions->functionsList.begin(); itOverridingFn != shaderWithOverridingFunctions->functionsList.end(); itOverridingFn++)
        {
            FunctionInstruction* shaderFunction = *itOverridingFn;
            if (shaderFunction->GetOverrideAttributeState() == FunctionInstruction::OverrideAttributeStateEnum::Defined)
                listOverridingFunctions.push_back(shaderFunction);
        }
        if (listOverridingFunctions.size() == 0) continue;

        //the overriding functions will override every functions having the same name, from the whole parents tree
        GetShaderFamilyTree(shaderWithOverridingFunctions, shaderFamilyTree);
        for (auto itOverridingFn = listOverridingFunctions.begin(); itOverridingFn != listOverridingFunctions.end(); itOverridingFn++)
        {
            FunctionInstruction* overridingFunction = *itOverridingFn;
            overridingFunction->SetOverrideAttributeState(FunctionInstruction::OverrideAttributeStateEnum::Processed);
            overridingFunction->SetOverridingFunction(nullptr); //the overriding function overrides itself by default (this could have been set to a different function previously)
            const string& overringFunctionName = overridingFunction->GetMangledName();

            for (auto itShaderFromTree = shaderFamilyTree.begin(); itShaderFromTree != shaderFamilyTree.end(); itShaderFromTree++)
            {
                ShaderClassData* shaderFromFamily = *itShaderFromTree;
                if (shaderFromFamily == shaderWithOverridingFunctions) continue;

                for (auto itFn = shaderFromFamily->functionsList.begin(); itFn != shaderFromFamily->functionsList.end(); itFn++)
                {
                    FunctionInstruction* aFunction = *itFn;
                    if (overringFunctionName == aFunction->GetMangledName())
                    {
                        //Got a function to be overriden !
                        aFunction->SetOverridingFunction(overridingFunction);
                    }
                }
            }
        }
    }

    if (errorMessages.size() > 0) return false;

    return true;
}

void SpxStreamRemapper::GetMixinBytecode(vector<uint32_t>& bytecodeStream)
{
    bytecodeStream.clear();
    bytecodeStream.insert(bytecodeStream.end(), spv.begin(), spv.end());
}

SpxStreamRemapper::FunctionInstruction* SpxStreamRemapper::GetFunctionForEntryPoint(std::string entryPointName)
{
    // Search for the entry point function (assume the first function with the name is the one)
    FunctionInstruction* entryPointFunction = nullptr;
    for (auto it = vecAllShaderFunctions.begin(); it != vecAllShaderFunctions.end(); it++)
    {
        FunctionInstruction* func = *it;
        string mangledFunctionName = func->GetMangledName();
        string unmangledFunctionName = mangledFunctionName.substr(0, mangledFunctionName.find('('));
        if (unmangledFunctionName == entryPointName)
        {
            entryPointFunction = func;
            //has the function been overriden?
            if (entryPointFunction->GetOverridingFunction() != nullptr) entryPointFunction = entryPointFunction->GetOverridingFunction();
            break;
        }
    }

    return entryPointFunction;
}

bool SpxStreamRemapper::GenerateSpvStageBytecode(ShadingStageEnum stage, string entryPointName, FunctionInstruction* entryPointFunction, SpvBytecode& output)
{
    if (status != SpxRemapperStatusEnum::MixinFinalized) return error("Invalid remapper status");
    if (entryPointFunction == nullptr) return error("Invalid entryPoint function");

    //==========================================================================================
    //==========================================================================================
    //save the current bytecode
    vector<uint32_t> bytecodeBackup;// = output.getWritableBytecodeStream();
    bytecodeBackup.insert(bytecodeBackup.end(), spv.begin(), spv.end());

    //==========================================================================================
    //==========================================================================================
    // Update the shader stage header
    if (!BuildAndSetShaderStageHeader(stage, entryPointFunction, entryPointName))
    {
        error("Error buiding the shader stage header");
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    //==========================================================================================
    //==========================================================================================
    //Clean and generate SPIRV bytecode using spirvbin_t class method
    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    dceFuncs(); //dce uncalled functions
    dceVars();  //dce unused function variables + decorations / name
    dceTypes(); //dce unused types

    strip();         //remove all strip bytecode
    stripDeadRefs(); //remove references to things we DCEed

    //change the range of all remaining IDs
    mapRemainder(); // map any unmapped IDs
    applyMap();     // Now remap each shader to the new IDs we've come up with

    //copy the spv bytecode into the output
    vector<uint32_t>& outputSpv = output.getWritableBytecodeStream();
    outputSpv.clear();
    outputSpv.insert(outputSpv.end(), spv.begin(), spv.end());

    //reset the initial spx bytecode
    spv.clear();
    spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());

    return true;
}

bool SpxStreamRemapper::SpxStreamRemapper::InitDefaultHeader()
{
    if (spv.size() > 0) {
        return error("Bytecode must by empty");
    }

    int uniqueId = 1;
    spv.push_back(MagicNumber);
    spv.push_back(Version);
    spv.push_back(builderNumber);
    spv.push_back(0);
    spv.push_back(0);

    //default memory model
    spv::Instruction memInst(0, 0, spv::OpMemoryModel);
    memInst.addImmediateOperand(spv::AddressingModelLogical);
    memInst.addImmediateOperand(spv::MemoryModelGLSL450);
    memInst.dump(spv);

    //default imports
    spv::Instruction defaultImport(uniqueId++, spv::NoType, spv::OpExtInstImport);
    defaultImport.addStringOperand("GLSL.std.450");
    defaultImport.dump(spv);

    setBound(uniqueId);

    return true;
}

bool SpxStreamRemapper::BuildAndSetShaderStageHeader(ShadingStageEnum stage, FunctionInstruction* entryFunction, string unmangledFunctionName)
{
    /*
    //capabilities
    //check which capabilities are required, check to merge capabilities from SPX bytecode
    spv::Instruction capInst(0, 0, spv::OpCapability);
    capInst.addImmediateOperand(spv::CapabilityShader);
    capInst.dump(stageHeader);

    //extensions?
    //spv::Instruction extInst(0, 0, spv::OpExtension);
    //extInst.addStringOperand(*it);
    //extInst.dump(stageHeader);

    //import
    spv::Instruction import(getUniqueId(), NoType, OpExtInstImport);
    import->addStringOperand(name);
    */

    if (entryFunction == nullptr){
        return error("Unknown entry function");
    }

    spv::ExecutionModel model = GetShadingStageExecutionMode(stage);
    if (model == spv::ExecutionModelMax)
    {
        return error("Unknown stage");
    }

    //opEntryPoint: set the stage model, and entry point
    vector<unsigned int> stageHeader;
    spv::Instruction entryPointInstr(spv::OpEntryPoint);
    entryPointInstr.addImmediateOperand(model);
    entryPointInstr.addIdOperand(entryFunction->GetResultId());
    entryPointInstr.addStringOperand(unmangledFunctionName.c_str());
    entryPointInstr.dump(stageHeader);

    //remove all current entry points
    {
        vector<range_t> vecStripRanges;
        process(
            [&](spv::Op opCode, unsigned start)
            {
                if (opCode == spv::OpEntryPoint)
                    stripInst(vecStripRanges, start);
                return true;
            },
            spx_op_fn_nop
        );
        stripBytecode(vecStripRanges);
    }

    //insert the stage header in the bytecode, after the header
    vector<unsigned int>::iterator it = spv.begin() + header_size;
    spv.insert(it, stageHeader.begin(), stageHeader.end());

    return true;
}

spv::ExecutionModel SpxStreamRemapper::GetShadingStageExecutionMode(ShadingStageEnum stage)
{
    switch (stage) {
    case ShadingStageEnum::Vertex:           return spv::ExecutionModelVertex;
    case ShadingStageEnum::Pixel:            return spv::ExecutionModelFragment;
    case ShadingStageEnum::TessControl:      return spv::ExecutionModelTessellationControl;
    case ShadingStageEnum::TessEvaluation:   return spv::ExecutionModelTessellationEvaluation;
    case ShadingStageEnum::Geometry:         return spv::ExecutionModelGeometry;
    case ShadingStageEnum::Compute:          return spv::ExecutionModelGLCompute;
    default:
        return spv::ExecutionModelMax;
    }
}

bool SpxStreamRemapper::BuildTypesAndConstsHashmap(unordered_map<uint32_t, pairIdPos>& mapHashPos)
{
    mapHashPos.clear();

    //We build hashmap table for all types and consts
    //except for OpTypeXlslShaderClass types: (this type is only informational, never used as a type or result)
    process(
        [&](spv::Op opCode, unsigned start)
        {
            spv::Id id = unused;
            if (opCode != spv::OpTypeXlslShaderClass)
            {
                if (isConstOp(opCode))
                {
                    id = asId(start + 2);
                }
                else if (isTypeOp(opCode))
                {
                    id = asId(start + 1);
                }
            }

            if (id != unused)
            {
                const uint32_t hashval = hashType(start);
#ifdef XKSLANG_DEBUG_MODE
                if (mapHashPos.find(hashval) != mapHashPos.end())
                {
                    // Warning: might cause some conflicts sometimes?
                    //return error(string("2 types have the same hashmap value. Ids: ") + to_string(mapHashPos[hashval].first) + string(", ") + to_string(id));
                    id = unused;  //by precaution we invalidate the id: we should not choose between them
                }
#endif
                mapHashPos[hashval] = pairIdPos(id, start);
            }
            return true;
        },
        spx_op_fn_nop
    );

    return true;
}

//Update the start and end position for all objects
bool SpxStreamRemapper::UpdateAllObjectsPositionInTheBytecode()
{
    //Parse the list of all object data
    vector<ParsedObjectData> listParsedObjectsData;
    bool res = BuildDeclarationNameMapsAndObjectsDataList(listParsedObjectsData);
    if (!res) {
        return error("Failed to build maps");
    }

    uint32_t maxResultId = bound();
    int countParsedObjects = listParsedObjectsData.size();
    for (int i = 0; i < countParsedObjects; ++i)
    {
        ParsedObjectData& parsedData = listParsedObjectsData[i];
        spv::Id resultId = parsedData.resultId;

        if (resultId <= 0 || resultId == spv::NoResult || resultId >= maxResultId)
            return error(string("The parsed object has an invalid resultId: ") + to_string(resultId));
        if (listAllObjects[resultId] == nullptr)
            return error(string("The parsed object is not registerd in the list of all objects. ResultId: ") + to_string(resultId));

        listAllObjects[resultId]->SetBytecodeRangePositions(parsedData.bytecodeStartPosition, parsedData.bytecodeEndPosition);
    }

    return true;
}

bool SpxStreamRemapper::UpdateAllMaps()
{
    //Parse the list of all object data
    vector<ParsedObjectData> listParsedObjectsData;
    bool res = this->BuildDeclarationNameMapsAndObjectsDataList(listParsedObjectsData);
    if (!res) {
        return error("Failed to build maps");
    }

    //Resize our vector of ALL objects
    uint32_t maxResultId = bound();
    if (maxResultId < listAllObjects.size()){
        return error("We lost some objects since last update");
    }
    listAllObjects.resize(maxResultId, nullptr);

    //======================================================================================================
    //create and store all NEW objects in corresponding maps
    vector<bool> vectorIdsToDecorate;
    vectorIdsToDecorate.resize(maxResultId, false);

    int countParsedObjects = listParsedObjectsData.size();
    for (int i = 0; i < countParsedObjects; ++i)
    {
        ParsedObjectData& parsedData = listParsedObjectsData[i];
        spv::Id resultId = parsedData.resultId;

        if (resultId <= 0 || resultId == spv::NoResult || resultId >= maxResultId)
            return error(string("The parsed object has an invalid resultId: ") + to_string(resultId));

        if (listAllObjects[resultId] != nullptr)
        {
            //the object already exist, we just update its position in the bytecode
            listAllObjects[resultId]->SetBytecodeRangePositions(parsedData.bytecodeStartPosition, parsedData.bytecodeEndPosition);
        }
        else
        {
            ObjectInstructionBase* newObject = CreateAndAddNewObjectFor(parsedData);
            if (newObject == nullptr) {
                return error("Failed to create the PSX objects from parsed data");
            }
            vectorIdsToDecorate[resultId] = true;
        }
    }

    //decorate all objects
    res = DecorateObjects(vectorIdsToDecorate);
    if (!res) {
        return error("Failed to decorate objects");
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::BuildAllMaps()
{
    //cout << "BuildAllMaps!!" << endl;

    ReleaseAllMaps();
    
    //Parse the list of all object data
    vector<ParsedObjectData> listParsedObjectsData;
    bool res = this->BuildDeclarationNameMapsAndObjectsDataList(listParsedObjectsData);
    if (!res) {
        return error("Failed to build maps");
    }

    //======================================================================================================
    //create and store all objects in corresponding maps
    int maxResultId = bound();
    listAllObjects.resize(maxResultId, nullptr);
    vector<bool> vectorIdsToDecorate;
    vectorIdsToDecorate.resize(maxResultId, false);

    int countParsedObjects = listParsedObjectsData.size();
    for (int i = 0; i < countParsedObjects; ++i)
    {
        ParsedObjectData& parsedData = listParsedObjectsData[i];
        
        ObjectInstructionBase* newObject = CreateAndAddNewObjectFor(parsedData);
        if (newObject == nullptr) {
            return error("Failed to create the PSX objects from parsed data");
        }

        vectorIdsToDecorate[newObject->GetResultId()] = true;
    }

    if (errorMessages.size() > 0) return false;

    //decorate all objects
    res = DecorateObjects(vectorIdsToDecorate);
    if (!res) {
        return error("Failed to decorate objects");
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::DecorateObjects(vector<bool>& vectorIdsToDecorate)
{
    //======================================================================================================
    // Decorate objects attributes and relations
    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode)
            {
                case spv::Op::OpBelongsToShader:
                {
                    const spv::Id shaderId = asId(start + 1);
                    const spv::Id objectId = asId(start + 2);

                    if (shaderId < 0 || shaderId >= vectorIdsToDecorate.size()) return true;
                    if (!vectorIdsToDecorate[shaderId]) return true;

                    ShaderClassData* shaderOwner = GetShaderById(shaderId);
                    if (shaderOwner == nullptr) { error(string("undeclared shader owner for Id: ") + to_string(shaderId)); break; }

                    FunctionInstruction* function = GetFunctionById(objectId);
                    if (function != nullptr) {
                        //a function is defined as being owned by a shader
#ifdef XKSLANG_DEBUG_MODE
                        if (function->GetShaderOwner() != nullptr) { error(string("The function already has a shader owner: ") + function->GetMangledName()); break; }
                        if (shaderOwner->HasFunction(function))
                        {
                            error(string("The shader: ") + shaderOwner->GetName() + string(" already possesses the function: ") + function->GetMangledName()); break;
                        }
                        function->SetFullName(shaderOwner->GetName() + string(".") + function->GetMangledName());
#endif
                        function->SetShaderOwner(shaderOwner);
                        shaderOwner->AddFunction(function);
                    }
                    else
                    {
                        //a type is defined as being owned by a shader
                        TypeInstruction* type = GetTypeById(objectId);
                        if (type != nullptr)
                        {
                            //A type belongs to a shader, we fetch the necessary data
                            //A shader type is defined by: the type, a pointer type to it, and a variable to access it
                            TypeInstruction* typePointer = GetTypePointingTo(type);
                            VariableInstruction* variable = GetVariablePointingTo(typePointer);

                            if (typePointer == nullptr) { error(string("cannot find the pointer type to the shader type: ") + type->GetName()); break; }
                            if (variable == nullptr) { error(string("cannot find the variable for shader type: ") + type->GetName()); break; }
#ifdef XKSLANG_DEBUG_MODE
                            if (type->GetShaderOwner() != nullptr) { error(string("The type already has a shader owner: ") + type->GetName()); break; }
                            if (shaderOwner->HasType(type)) { error(string("The shader: ") + shaderOwner->GetName() + string(" already possesses the type: ") + type->GetName()); break; }
#endif
                            ShaderTypeData* shaderType = new ShaderTypeData(type, typePointer, variable);
                            type->SetShaderOwner(shaderOwner);
                            shaderOwner->AddShaderType(shaderType);
                        }
                        else
                        {
                            error(string("unprocessed OpBelongsToShader instruction, invalid objectId: ") + to_string(objectId));
                        }
                    }
                    break;
                }

                case spv::OpShaderInheritance:
                {
                    //A shader inherits from some shader parents
                    const spv::Id shaderId = asId(start + 1);

                    if (shaderId < 0 || shaderId >= vectorIdsToDecorate.size()) return true;
                    if (!vectorIdsToDecorate[shaderId]) return true;

                    ShaderClassData* shader = GetShaderById(shaderId);
                    if (shader == nullptr) { error(string("undeclared shader for Id: ") + to_string(shaderId)); break; }

                    int countParents = asWordCount(start) - 2;
                    for (int p = 0; p < countParents; ++p)
                    {
                        const spv::Id parentShaderId = asId(start + 2 + p);
                        ShaderClassData* shaderParent = GetShaderById(parentShaderId);
                        if (shaderParent == nullptr) { error(string("undeclared parent shader for Id: ") + to_string(parentShaderId)); break; }

#ifdef XKSLANG_DEBUG_MODE
                        if (shader->HasParent(shaderParent)) { error(string("shader: ") + shader->GetName() + string(" already inherits from parent: ") + shaderParent->GetName()); break; }
#endif
                        shader->AddParent(shaderParent);
                    }
                    break;
                }

                case spv::OpShaderComposition:
                case spv::OpShaderArrayComposition:
                {
                    const spv::Id shaderId = asId(start + 1);

                    if (shaderId < 0 || shaderId >= vectorIdsToDecorate.size()) return true;
                    if (!vectorIdsToDecorate[shaderId]) return true;

                    bool isArray = opCode == spv::OpShaderArrayComposition ? true : false;
                    ShaderClassData* shaderCompositionOwner = GetShaderById(shaderId);
                    if (shaderCompositionOwner == nullptr) { error(string("undeclared shader id: ") + to_string(shaderId)); break; }

                    int compositionId = asId(start + 2);

                    const spv::Id shaderCompositionTypeId = asId(start + 3);
                    ShaderClassData* shaderCompositionType = GetShaderById(shaderCompositionTypeId);
                    if (shaderCompositionType == nullptr) { error(string("undeclared shader type id: ") + to_string(shaderCompositionTypeId)); break; }

                    int status = asId(start + 4);
                    ShaderComposition::ShaderCompositionStatusEnum compstatus = status == 0?
                        ShaderComposition::ShaderCompositionStatusEnum::Unresolved :
                        ShaderComposition::ShaderCompositionStatusEnum::Resolved;

                    const string compositionVariableName = literalString(start + 5);

                    ShaderComposition shaderComposition(compositionId, shaderCompositionOwner, shaderCompositionType, compositionVariableName, isArray, compstatus);
                    shaderCompositionOwner->AddComposition(shaderComposition);
                    break;
                }

                case spv::Op::OpMethodProperties:
                {
                    //a function is defined with some properties
                    const spv::Id functionId = asId(start + 1);

                    if (functionId < 0 || functionId >= vectorIdsToDecorate.size()) return true;
                    if (!vectorIdsToDecorate[functionId]) return true;

                    FunctionInstruction* function = GetFunctionById(functionId);
                    if (function == nullptr) { error(string("undeclared function id: ") + to_string(functionId)); break; }

                    int countProperties = asWordCount(start) - 2;
                    for (int a = 0; a < countProperties; ++a)
                    {
                        int prop = asId(start + 2 + a);
                        switch (prop)
                        {
                        case spv::XkslPropertyEnum::PropertyMethodOverride:
                            function->ParsedOverrideAttribute();
                            break;
                        }
                    }
                    break;
                }
            }

            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) return false;
    return true;
}

SpxStreamRemapper::ObjectInstructionBase* SpxStreamRemapper::CreateAndAddNewObjectFor(ParsedObjectData& parsedData)
{
    spv::Id resultId = parsedData.resultId;
    if (resultId <= 0 || resultId == spv::NoResult || resultId >= listAllObjects.size()) {
        error(string("The object has an invalid resultId: ") + to_string(resultId));
        return nullptr;
    }
    if (listAllObjects[resultId] != nullptr) {
        error(string("An object with the same resultId already exists. resultId: ") + to_string(resultId));
        return nullptr;
    }

    string declarationName;
    bool hasDeclarationName = GetDeclarationNameForId(parsedData.resultId, declarationName);

    bool declarationNameRequired = true;
    ObjectInstructionBase* newObject = nullptr;
    switch (parsedData.kind)
    {
        case ObjectInstructionTypeEnum::HeaderProperty:
        {
            declarationNameRequired = false;
            string headerPropName = "";
            switch (parsedData.opCode)
            {
            case spv::Op::OpExtInstImport:
                headerPropName = literalString(parsedData.bytecodeStartPosition + 2);
                break;
            default:
                error(string("HeaderProperty has an unknown OpCode: ") + to_string(parsedData.opCode));
                return nullptr;
            }
            newObject = new HeaderPropertyInstruction(parsedData, headerPropName, this);
            break;
        }
        case ObjectInstructionTypeEnum::Const:
        {
            declarationNameRequired = false;
            newObject = new ConstInstruction(parsedData, declarationName, this);
            break;
        }
        case ObjectInstructionTypeEnum::Shader:
        {
            declarationNameRequired = true;
            ShaderClassData* shader = new ShaderClassData(parsedData, declarationName, this);
            vecAllShaders.push_back(shader);
            newObject = shader;
            break;
        }
        case ObjectInstructionTypeEnum::Type:
        {
            declarationNameRequired = false;
            TypeInstruction* type = new TypeInstruction(parsedData, declarationName, this);
            if (isPointerTypeOp(parsedData.opCode))
            {
                //create the link to the type pointed by the pointer (already created at this stage)
                TypeInstruction* pointedType = GetTypeById(parsedData.targetId);
                if (pointedType == nullptr) {
                    error(string("Cannot find the typeId: ") + to_string(parsedData.targetId) + string(", pointed by pointer Id: ") + to_string(resultId));
                    delete type;
                    return nullptr;
                }
                type->SetTypePointed(pointedType);
            }
            newObject = type;

            break;
        }
        case ObjectInstructionTypeEnum::Variable:
        {
            declarationNameRequired = false;
            
            //create the link to the type pointed by the variable (already created at this stage)
            TypeInstruction* pointedType = GetTypeById(parsedData.typeId);
            if (pointedType == nullptr) {
                error(string("Cannot find the typeId: ") + to_string(parsedData.typeId) + string(", pointed by variable Id: ") + to_string(resultId));
                return nullptr;
            }

            VariableInstruction* variable = new VariableInstruction(parsedData, declarationName, this);
            variable->SetTypePointed(pointedType);
            newObject = variable;
            break;
        }
        case ObjectInstructionTypeEnum::Function:
        {
            declarationNameRequired = false;  //some functions can be declared outside a shader definition, they don't belong to a shader then
            FunctionInstruction* function = new FunctionInstruction(parsedData, declarationName, this);

            if (hasDeclarationName && declarationName.size() > 0)
            {
                vecAllShaderFunctions.push_back(function);
            }

            newObject = function;
            break;
        }
    }

    listAllObjects[resultId] = newObject;

    if (newObject == nullptr) {
        error("Unknown parsed data kind");
        return nullptr;
    }

    if (declarationNameRequired && !hasDeclarationName) {
        error("Object requires a declaration name");
        return nullptr;
    }

    return newObject;
}

bool SpxStreamRemapper::ComputeShadersLevel()
{
    //===================================================================================================================
    // Set the shader levels (also detects cyclic shader inheritance)
    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
        (*itsh)->level = -1;

    bool allShaderSet = false;
    while (!allShaderSet)
    {
        allShaderSet = true;
        bool anyShaderUpdated = false;

        for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
        {
            ShaderClassData* shader = *itsh;
            if (shader->level != -1) continue;  //shader already set

            if (shader->parentsList.size() == 0)
            {
                shader->level = 0; //shader has no parent
                anyShaderUpdated = true;
                continue;
            }

            int maxParentLevel = -1;
            for (unsigned int p = 0; p<shader->parentsList.size(); ++p)
            {
                int parentLevel = shader->parentsList[p]->level;
                if (parentLevel == -1)
                {
                    //parent not set yet: got to wait
                    allShaderSet = false;
                    maxParentLevel = -1;
                    break;
                }
                if (parentLevel > maxParentLevel) maxParentLevel = parentLevel;
            }

            if (maxParentLevel >= 0)
            {
                shader->level = maxParentLevel + 1; //shader level
                anyShaderUpdated = true;
            }
        }

        if (!anyShaderUpdated)
        {
            return error("Cyclic inheritance detected among shaders");
        }
    }

    return true;
}

bool SpxStreamRemapper::BuildDeclarationNameMapsAndObjectsDataList(vector<ParsedObjectData>& listParsedObjectsData)
{
    mapDeclarationName.clear();
    idPosR.clear();
    listParsedObjectsData.clear();

    int fnStart = 0;
    spv::Id fnResId = spv::NoResult;
    spv::Id fnTypeId = spv::NoResult;
    spv::Op fnOpCode;
    process(
        [&](spv::Op opCode, unsigned start)
        {
            unsigned end = start + asWordCount(start);

            //Fill idPosR map (used by hashType)
            unsigned word = start+1;
            spv::Id typeId = spv::NoType;
            spv::Id resultId = spv::NoResult;
            if (spv::InstructionDesc[opCode].hasType())
                typeId = asId(word++);
            if (spv::InstructionDesc[opCode].hasResult()) {
                resultId = asId(word++);
                idPosR[resultId] = start;
            }

            if (opCode == spv::Op::OpName)
            {
            /*
#ifdef XKSLANG_DEBUG_MODE
                const spv::Id target = asId(start + 1);
                const string name = literalString(start + 2);

                if (mapDeclarationName.find(target) == mapDeclarationName.end())
                    mapDeclarationName[target] = name;
#endif*/
            }
            else if (opCode == spv::Op::OpDeclarationName)
            {
                const spv::Id target = asId(start + 1);
                const string  name = literalString(start + 2);
                mapDeclarationName[target] = name;
            }
            else if (isConstOp(opCode))
            {
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Const, opCode, resultId, typeId, start, end));
            }
            else if (isTypeOp(opCode))
            {
                if (opCode == spv::OpTypeXlslShaderClass)
                {
                    listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Shader, opCode, resultId, typeId, start, end));
                }
                else
                {
                    ParsedObjectData data = ParsedObjectData(ObjectInstructionTypeEnum::Type, opCode, resultId, typeId, start, end);
                    if (isPointerTypeOp(opCode))
                    {
                        spv::Id targetId = asId(start + 3);
                        data.SetTargetId(targetId);
                    }
                    listParsedObjectsData.push_back(data);
                }
            }
            else if (isVariableOp(opCode))
            {
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Variable, opCode, resultId, typeId, start, end));
            }
            else if (opCode == spv::Op::OpFunction)
            {
                if (fnStart != 0) error("nested function found");
                fnStart = start;
                fnTypeId = typeId;
                fnResId = resultId;
                fnOpCode = opCode;
            }
            else if (opCode == spv::Op::OpFunctionEnd)
            {
                if (fnStart == 0) error("function end without function start");
                if (fnResId == spv::NoResult) error("function has no result iD");
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Function, fnOpCode, fnResId, fnTypeId, fnStart, end));
                fnStart = 0;
            }
            else if (opCode == spv::Op::OpExtInstImport)
            {
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::HeaderProperty, opCode, resultId, typeId, start, end));
            }
            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) return false;
    return true;
}

//===========================================================================================================================//
//===========================================================================================================================//
string SpxStreamRemapper::GetDeclarationNameForId(spv::Id id)
{
    auto it = mapDeclarationName.find(id);
    if (it == mapDeclarationName.end())
    {
        error(string("Id: ") + to_string(id) + string(" has no declaration name"));
        return string("");
    }
    return it->second;
}

bool SpxStreamRemapper::GetDeclarationNameForId(spv::Id id, string& name)
{
    auto it = mapDeclarationName.find(id);
    if (it == mapDeclarationName.end())
        return false;

    name = it->second;
    return true;
}

SpxStreamRemapper::HeaderPropertyInstruction* SpxStreamRemapper::GetHeaderPropertyInstructionByOpCodeAndName(const spv::Op opCode, const string& name)
{
    if (name.size() == 0) return nullptr;

    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::HeaderProperty && obj->GetOpCode() == opCode && obj->GetName() == name)
        {
            HeaderPropertyInstruction* headerProp = dynamic_cast<HeaderPropertyInstruction*>(obj);
            return headerProp;
        }
    }
    return nullptr;
}

SpxStreamRemapper::ShaderClassData* SpxStreamRemapper::GetShaderByName(const string& name)
{
    if (name.size() == 0) return nullptr;

    for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); ++it)
    {
        ShaderClassData* shader = *it;
        if (shader->GetName() == name)
        {
            return shader;
        }
    }
    return nullptr;
}

SpxStreamRemapper::ShaderComposition* SpxStreamRemapper::GetCompositionById(spv::Id shaderId, int compositionId)
{
    ShaderClassData* shader = GetShaderById(shaderId);
    if (shader == nullptr) return nullptr;
    return shader->GetShaderCompositionById(compositionId);
}

SpxStreamRemapper::ShaderClassData* SpxStreamRemapper::GetShaderById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Shader)
    {
        ShaderClassData* shader = dynamic_cast<ShaderClassData*>(obj);
        return shader;
    }
    return nullptr;
}

SpxStreamRemapper::VariableInstruction* SpxStreamRemapper::GetVariableByName(const string& name)
{
    if (name.size() == 0) return nullptr;

    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Variable && obj->GetName() == name)
        {
            VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
            return variable;
        }
    }
    return nullptr;
}

SpxStreamRemapper::ObjectInstructionBase* SpxStreamRemapper::GetObjectById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];
    return obj;
}

SpxStreamRemapper::FunctionInstruction* SpxStreamRemapper::GetFunctionById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Function)
    {
        FunctionInstruction* function = dynamic_cast<FunctionInstruction*>(obj);
        return function;
    }
    return nullptr;
}

SpxStreamRemapper::TypeInstruction* SpxStreamRemapper::GetTypeById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
    {
        TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
        return type;
    }
    return nullptr;
}

SpxStreamRemapper::VariableInstruction* SpxStreamRemapper::GetVariableById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Variable)
    {
        VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
        return variable;
    }
    return nullptr;
}

SpxStreamRemapper::TypeInstruction* SpxStreamRemapper::GetTypePointingTo(TypeInstruction* targetType)
{
    TypeInstruction* res = nullptr;
    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
        {
            TypeInstruction* aType = dynamic_cast<TypeInstruction*>(obj);
            if (aType->GetTypePointed() == targetType)
            {
                if (res != nullptr) error("found 2 types pointing to the same type");
                res = aType;
            }
        }
    }
    return res;
}

SpxStreamRemapper::VariableInstruction* SpxStreamRemapper::GetVariablePointingTo(TypeInstruction* targetType)
{
    VariableInstruction* res = nullptr;
    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Variable)
        {
            VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
            if (variable->GetTypePointed() == targetType)
            {
                if (res != nullptr) error("found 2 variables pointing to the same type");
                res = variable;
            }
        }
    }
    return res;
}

void SpxStreamRemapper::stripBytecode(vector<range_t>& ranges)
{
    if (ranges.empty()) // nothing to do
        return;

    // Sort strip ranges in order of traversal
    sort(ranges.begin(), ranges.end());

    // Allocate a new binary big enough to hold old binary
    // We'll step this iterator through the strip ranges as we go through the binary
    auto strip_it = ranges.begin();

    int strippedPos = 0;
    for (unsigned word = 0; word < unsigned(spv.size()); ++word) {
        if (strip_it != ranges.end() && word >= strip_it->second)
            ++strip_it;

        if (strip_it == ranges.end() || word < strip_it->first || word >= strip_it->second)
            spv[strippedPos++] = spv[word];
    }

    spv.resize(strippedPos);
}
