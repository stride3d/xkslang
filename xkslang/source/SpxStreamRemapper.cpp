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

static const string globalStreamStructDefaultName = "globalStreams";
static const string globalVarOnStreamStructDefaultName = "globalStreams_var";
static const string functionInputVariableDefaultName = "__input__";
static const string functionOutputVariableDefaultName = "__output__";
static const string functionStreamVariableDefaultName = "_streams";

//===================================================================================================//
BytecodeChunk& BytecodeUpdateController::InsertNewBytecodeChunckAt(unsigned int position, unsigned int countBytesToOverlap)
{
    auto itListPos = listSortedChunksToInsert.begin();
    while (itListPos != listSortedChunksToInsert.end())
    {
        if (position > itListPos->insertionPos) break;
        itListPos++;
    }
    itListPos = listSortedChunksToInsert.insert(itListPos, BytecodeChunk(position, countBytesToOverlap));
    return *itListPos;
}

//===================================================================================================//
void SpxStreamRemapper::copyMessagesTo(vector<string>& list)
{
    list.insert(list.end(), errorMessages.begin(), errorMessages.end());
}

bool SpxStreamRemapper::error(const string& txt)
{
    errorMessages.push_back(txt);
    return false;
}

//===================================================================================================//
//static const auto spx_inst_fn_nop = [](spv::Op, unsigned) { return false; };
//static const auto spx_op_fn_nop = [](spv::Id&) {};

unsigned int SpxStreamRemapper::currentMergeOperationId = 0;

unsigned int SpxStreamRemapper::GetUniqueMergeOperationId()
{
    return SpxStreamRemapper::currentMergeOperationId++;
}

void SpxStreamRemapper::ResetMergeOperationId()
{
    SpxStreamRemapper::currentMergeOperationId = 0;
}
//===================================================================================================//

SpxStreamRemapper::SpxStreamRemapper(int verbose) : spirvbin_t(verbose)
{
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
                    clonedType->SetShaderOwner(clonedShader);
                    clonedTypePointer->SetShaderOwner(clonedShader);
                    clonedVariable->SetShaderOwner(clonedShader);
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
                compositionToClone.isArray, compositionToClone.countInstances);
            //clonedComposition.instantiatedShader = compositionToClone.instantiatedShader == nullptr ? nullptr : clonedSpxRemapper->GetShaderById(compositionToClone.instantiatedShader->GetId());
            clonedShader->AddComposition(clonedComposition);
        }
    }

    for (auto it = vecAllFunctions.begin(); it != vecAllFunctions.end(); it++)
    {
        FunctionInstruction* function = clonedSpxRemapper->GetFunctionById((*it)->GetId());
        clonedSpxRemapper->vecAllFunctions.push_back(function);
    }

    clonedSpxRemapper->status = status;
    for (auto it = idPosR.begin(); it != idPosR.end(); it++)
        clonedSpxRemapper->idPosR[it->first] = it->second;
    for (auto it = errorMessages.begin(); it != errorMessages.end(); it++)
        clonedSpxRemapper->errorMessages.push_back(*it);
    
    return clonedSpxRemapper;
}

bool SpxStreamRemapper::RemoveShaderTypeFromBytecodeAndData(ShaderTypeData* shaderTypeToRemove, vector<range_t>& vecStripRanges)
{
    ShaderClassData* shaderOwner = shaderTypeToRemove->type->shaderOwner;

    vector<bool> listIdsRemoved;
    listIdsRemoved.resize(bound(), false);

    //remove the type belonging to the shader
    {
        bool typeFound = false;
        for (auto itt = shaderOwner->shaderTypesList.begin(); itt != shaderOwner->shaderTypesList.end(); itt++)
        {
            ShaderTypeData* aShaderType = *itt;
            if (shaderTypeToRemove != aShaderType) continue;

            spv::Id id;
            TypeInstruction* type = shaderTypeToRemove->type;
            TypeInstruction* pointerToType = shaderTypeToRemove->pointerToType;
            VariableInstruction* variable = shaderTypeToRemove->variable;

            stripInst(vecStripRanges, type->GetBytecodeStartPosition(), type->GetBytecodeEndPosition());
            id = type->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;
            listIdsRemoved[id] = true;

            stripInst(vecStripRanges, pointerToType->GetBytecodeStartPosition(), pointerToType->GetBytecodeEndPosition());
            id = pointerToType->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;
            listIdsRemoved[id] = true;

            stripInst(vecStripRanges, variable->GetBytecodeStartPosition(), variable->GetBytecodeEndPosition());
            id = variable->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;
            listIdsRemoved[id] = true;

            //remove the element
            typeFound = true;
            iter_swap(itt, shaderOwner->shaderTypesList.end() - 1);
            shaderOwner->shaderTypesList.pop_back();
            break;
        }
        if (!typeFound) return error(string("Failed to find the shaderType to remove: ") + shaderTypeToRemove->type->GetName());
    }

    //remove all decorates related to the objects we removed
    {
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                //case spv::OpTypeXlslShaderClass:  //already removed above
                case spv::OpName:
                case spv::OpMemberName:
                case spv::OpDecorate:
                case spv::OpMemberDecorate:
                case spv::OpDeclarationName:
                case spv::OpShaderInheritance:
                case spv::OpShaderCompositionDeclaration:
                case spv::OpShaderCompositionInstance:
                case spv::OpBelongsToShader:
                case spv::OpMethodProperties:
                case spv::OpMemberProperties:
                case spv::OpMemberSemanticName:
                {
                    const spv::Id id = asId(start + 1);
                    if (listIdsRemoved[id]) stripInst(vecStripRanges, start, start + wordCount);
                    break;
                }

                case spv::OpTypeXlslShaderClass:
                {
                    //every decorate and previous Op have been declared before type declaration, we can stop here
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }
    }

    return true;
}

bool SpxStreamRemapper::RemoveShaderFromBytecodeAndData(ShaderClassData* shaderToRemove, vector<range_t>& vecStripRanges)
{
    spv::Id shaderId = shaderToRemove->GetId();
    if (GetShaderById(shaderId) != shaderToRemove) return error(string("Failed to find the shader to remove: ") + shaderToRemove->GetName());

    vector<bool> listIdsRemoved;
    listIdsRemoved.resize(bound(), false);

    //remove the shader from vecAllShaders: swap the shader with the latest elements (to remove it without cost, and to check if it does belong in the list)
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
        for (unsigned int i = 0; i < vecAllFunctions.size(); ++i)
        {
            FunctionInstruction* function = vecAllFunctions[i];
            if (function->shaderOwner == shaderToRemove)
            {
                countFunctionsRemoved++;

                if (i < vecAllFunctions.size() - 1)
                    vecAllFunctions[i] = vecAllFunctions[vecAllFunctions.size() - 1];
                vecAllFunctions.pop_back();

                stripInst(vecStripRanges, function->GetBytecodeStartPosition(), function->GetBytecodeEndPosition());
                spv::Id id = function->GetId();
                delete listAllObjects[id];
                listAllObjects[id] = nullptr;
                listIdsRemoved[id] = true;
            }
        }
        if (countFunctionsRemoved != shaderToRemove->GetCountFunctions())
            return error(string("Discrepancy between shader list of functions and list of all shader functions for shader: ") + shaderToRemove->GetName());
    }

    //remove all types belonging to the shader
    {
        for (unsigned int t = 0; t < shaderToRemove->shaderTypesList.size(); ++t)
        {
            ShaderTypeData* shaderTypeToRemove = shaderToRemove->shaderTypesList[t];

            spv::Id id;
            TypeInstruction* type = shaderTypeToRemove->type;
            TypeInstruction* pointerToType = shaderTypeToRemove->pointerToType;
            VariableInstruction* variable = shaderTypeToRemove->variable;

            stripInst(vecStripRanges, type->GetBytecodeStartPosition(), type->GetBytecodeEndPosition());
            id = type->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;
            listIdsRemoved[id] = true;

            stripInst(vecStripRanges, pointerToType->GetBytecodeStartPosition(), pointerToType->GetBytecodeEndPosition());
            id = pointerToType->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;
            listIdsRemoved[id] = true;

            stripInst(vecStripRanges, variable->GetBytecodeStartPosition(), variable->GetBytecodeEndPosition());
            id = variable->GetId();
            delete listAllObjects[id];
            listAllObjects[id] = nullptr;
            listIdsRemoved[id] = true;
        }
    }

    //remove the shader's bytecode and object
    {
        stripInst(vecStripRanges, shaderToRemove->GetBytecodeStartPosition());
        spv::Id id = shaderToRemove->GetId();
        delete listAllObjects[id];
        listAllObjects[id] = nullptr;
        listIdsRemoved[id] = true;
    }

    //remove all decorates related to the objects we removed
    {
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                //case spv::OpTypeXlslShaderClass:  //already removed above
                case spv::OpName:
                case spv::OpMemberName:
                case spv::OpDecorate:
                case spv::OpMemberDecorate:
                case spv::OpDeclarationName:
                case spv::OpShaderInheritance:
                case spv::OpShaderCompositionDeclaration:
                case spv::OpShaderCompositionInstance:
                case spv::OpBelongsToShader:
                case spv::OpMethodProperties:
                case spv::OpMemberProperties:
                case spv::OpMemberSemanticName:
                {
                    const spv::Id id = asId(start + 1);
                    if (listIdsRemoved[id]) stripInst(vecStripRanges, start, start + wordCount);
                    break;
                }

                case spv::OpTypeXlslShaderClass:
                {
                    //every decorate and previous Op have been declared before type declaration, we can stop here
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }
    }

    return true;
}

void SpxStreamRemapper::ReleaseAllMaps()
{
    unsigned int size = listAllObjects.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        if (listAllObjects[i] != nullptr) delete listAllObjects[i];
    }

    listAllObjects.clear();
    vecAllShaders.clear();
    vecAllFunctions.clear();
    mapDeclarationName.clear();
}

bool SpxStreamRemapper::MixWithShadersFromBytecode(const SpxBytecode& sourceBytecode, const vector<string>& nameOfShadersToMix)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    if (nameOfShadersToMix.size() == 0) {
        return error("List of shader is empty");
    }

    //=============================================================================================================
    // First mixin: set a default header to the bytecode
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

    //===============================================================================================================================================
    //===============================================================================================================================================
    //parse and init the bytecode to merge
    SpxStreamRemapper bytecodeToMerge;
    if (!bytecodeToMerge.SetBytecode(sourceBytecode)) return false;

    if (!bytecodeToMerge.BuildAllMaps()) {
        bytecodeToMerge.copyMessagesTo(errorMessages);
        return error(string("Error parsing the bytecode: ") + sourceBytecode.GetName());
    }
    for (auto itsh = bytecodeToMerge.vecAllShaders.begin(); itsh != bytecodeToMerge.vecAllShaders.end(); itsh++)
        (*itsh)->flag1 = 0;

    //===============================================================================================================================================
    //===============================================================================================================================================
    // Build the list of all shaders we want to merge

    //retrieve the shader object from source bytecode
    vector<ShaderClassData*> listShader;
    for (unsigned int is = 0; is < nameOfShadersToMix.size(); ++is)
    {
        const string& nameShaderToMix = nameOfShadersToMix[is];
        ShaderClassData* shaderToMerge = bytecodeToMerge.GetShaderByName(nameShaderToMix);
        if (shaderToMerge == nullptr) {
            return error(string("Shader not found in source bytecode: ") + nameShaderToMix);
        }
        listShader.push_back(shaderToMerge);
    }

    vector<ShaderClassData*> shadersFullDependencies;
    if (!SpxStreamRemapper::GetShadersFullDependencies(&bytecodeToMerge, listShader, shadersFullDependencies)){
        bytecodeToMerge.copyMessagesTo(errorMessages);
        return error(string("Failed to get the shaders dependencies"));
    }

    //We won't add a shader if it already exists in the destination bytecode
    vector<ShaderToMergeData> listShadersToMerge;
    for (auto its = shadersFullDependencies.begin(); its != shadersFullDependencies.end(); its++)
    {
        ShaderClassData* aShaderToMerge = *its;
        if (this->GetShaderByName(aShaderToMerge->GetName()) == nullptr)
        {
            listShadersToMerge.push_back(ShaderToMergeData(aShaderToMerge));
        }
    }

    if (listShadersToMerge.size() == 0){
        //no new shader to merge, can quit there
        status = SpxRemapperStatusEnum::WaitingForMixin;
        return true;
    }

    //===============================================================================================================================================
    //===============================================================================================================================================
    // Merge the shaders
    if (!MergeShadersIntoBytecode(bytecodeToMerge, listShadersToMerge, "")){
        return error("Failed tomerge the shaders");
    }

    //===============================================================================================================================================
    //===============================================================================================================================================
    //retrieve the merged shaders from the destination bytecode
    vector<ShaderClassData*> listShadersMerged;
    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is].shader;
        ShaderClassData* shaderMerged = shaderToMerge->tmpClonedShader; //GetShaderByName(shaderToMerge->GetName());
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

bool SpxStreamRemapper::MergeShadersIntoBytecode(SpxStreamRemapper& bytecodeToMerge, const vector<ShaderToMergeData>& listShadersToMerge, string allInstancesPrefixToAdd)
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
    finalRemapTable.resize(bytecodeToMerge.bound(), spv::spirvbin_t::unused);
    //keep the list of merged Ids so that we can look for their name or decorate
    vector<bool> listAllNewIdMerged;
    vector<bool> listIdsWhereToAddNamePrefix;
    listAllNewIdMerged.resize(bytecodeToMerge.bound(), false);
    listIdsWhereToAddNamePrefix.resize(bytecodeToMerge.bound(), false);
    vector<pair<spv::Id, spv::Id>> listOverridenFunctionMergedToBeRemappedWithMergedFunctions;

    for (unsigned int is=0; is<listShadersToMerge.size(); ++is)
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
            if (finalRemapTable[type->GetId()] != spv::spirvbin_t::unused) error(string("id: ") + to_string(type->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[pointerToType->GetId()] != spv::spirvbin_t::unused) error(string("id: ") + to_string(pointerToType->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[variable->GetId()] != spv::spirvbin_t::unused) error(string("id: ") + to_string(variable->GetId()) + string(" has already been remapped"));
#endif

            finalRemapTable[type->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, type->GetBytecodeStartPosition());
            finalRemapTable[pointerToType->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, pointerToType->GetBytecodeStartPosition());
            finalRemapTable[variable->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, variable->GetBytecodeStartPosition());

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
            if (finalRemapTable[resultId] != spv::spirvbin_t::unused) error(string("id: ") + to_string(resultId) + string(" has already been remapped"));
#endif

            finalRemapTable[resultId] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge, shaderToMerge->GetBytecodeStartPosition());

            if (instantiateTheShader) {
                listIdsWhereToAddNamePrefix[resultId] = true;
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
                        if (finalRemapTable[resultId] != spv::spirvbin_t::unused) error(string("id: ") + to_string(resultId) + string(" has already been remapped"));
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
        unsigned int len = finalRemapTable.size();
        for (unsigned int i = 0; i < len; ++i)
        {
            if (finalRemapTable[i] != spv::spirvbin_t::unused) listAllNewIdMerged[i] = true;
        }
    }

    //Populate vecNewShadersDecorationPossesingIds with some decorate instructions which might contains unmapped IDs
    vector<uint32_t> vecXkslDecorationsPossesingIds;
    {
        unsigned int start = bytecodeToMerge.header_size;
        const unsigned int end = bytecodeToMerge.spv.size();
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
        const unsigned int end = bytecodeToCheckForUnmappedIds.size();
        string errorMsg;
        while (start < end)
        {
            listIds.clear();
            if (!parseInstruction(bytecodeToCheckForUnmappedIds, start, opCode, wordCount, typeId, resultId, listIds, errorMsg)){
                error(errorMsg);
                return error("Merge shaders. Error parsing bytecodeToCheckForUnmappedIds instructions");
            }

            if (typeId != spv::spirvbin_t::unused) listIds.push_back(typeId);
            listIdsLen = listIds.size();
            for (unsigned int i = 0; i < listIdsLen; ++i)
            {
                const spv::Id id = listIds[i];
                if (finalRemapTable[id] == spv::spirvbin_t::unused)
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
            if (finalRemapTable[unmappedId] != spv::spirvbin_t::unused)
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

                        if (idOfSameTypeFromDestinationBytecode == spv::spirvbin_t::unused)
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
                    if (!parseInstruction(bytecodeToCheckForUnmappedIds, 0, opCode, wordCount, typeId, resultId, listIds, errorMsg)){
                        error(errorMsg);
                        return error("Error parsing bytecodeToCheckForUnmappedIds instructions");
                    }

                    if (typeId != spv::spirvbin_t::unused) listIds.push_back(typeId);
                    listIdsLen = listIds.size();
                    bool canAddTheInstruction = true;
                    for (unsigned int i = 0; i < listIdsLen; ++i)
                    {
                        const spv::Id anotherId = listIds[i];
#ifdef XKSLANG_DEBUG_MODE
                        if (anotherId == unmappedId) return error(string("anotherId == unmappedId: ") + to_string(anotherId) + string(". This should be impossible (bytecode is invalid)"));
#endif
                        if (finalRemapTable[anotherId] == spv::spirvbin_t::unused)
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
        const unsigned int end = bytecodeToMerge.spv.size();
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
                case spv::OpMemberProperties:
                case spv::OpMemberSemanticName:
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
        if (!remapAllIds(vecHeaderPropertiesToMerge, 0, vecHeaderPropertiesToMerge.size(), finalRemapTable))
            return error("remapAllIds failed on vecHeaderPropertiesToMerge");
    }
    if (!remapAllIds(vecTypesConstsAndVariablesToMerge, 0, vecTypesConstsAndVariablesToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecTypesConstsAndVariablesToMerge");
    if (!remapAllIds(vecXkslDecoratesToMerge, 0, vecXkslDecoratesToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecXkslDecoratesToMerge");
    if (!remapAllIds(vecNamesAndDecorateToMerge, 0, vecNamesAndDecorateToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecNamesAndDecorateToMerge");
    if (!remapAllIds(vecFunctionsToMerge, 0, vecFunctionsToMerge.size(), finalRemapTable))
        return error("remapAllIds failed on vecFunctionsToMerge");
    //vecFunctionsToMerge.processOnFullBytecode(
    //    spx_inst_fn_nop,
    //    [&](spv::Id& id)
    //    {
    //        spv::Id newId = finalRemapTable[id];
    //        if (newId == spv::spirvbin_t::unused) error(string("Invalid remapper Id: ") + to_string(id));
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
                case spv::OpShaderCompositionDeclaration:
                case spv::OpShaderCompositionInstance:
                case spv::OpMethodProperties:
                case spv::OpMemberProperties:
                case spv::OpMemberSemanticName:
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
    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is].shader;
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
    //Find the shader from source bytecode to be instantiated
    ShaderClassData* shaderCompositionTarget = this->GetShaderByName(shaderName);
    if (shaderCompositionTarget == nullptr) return error(string("No shader exists in destination bytecode with the name: ") + shaderName);

    //find the shader's composition target
    ShaderComposition* compositionTarget = shaderCompositionTarget->GetShaderCompositionByName(variableName);
    if (compositionTarget == nullptr) return error(string("No composition exists in shader: ") + shaderName + string(", with the name: ") + variableName);

    if (!compositionTarget->isArray && compositionTarget->countInstances > 0) return error(string("The composition has already been instanciated"));

    int compositionTargetId = compositionTarget->compositionShaderId;

    ShaderClassData* shaderTypeToInstantiate = compositionTarget->shaderType;
    if (shaderTypeToInstantiate == nullptr) return error("The composition does not define any shader type");

    ShaderClassData* mainShaderTypeMergedToMergeFromSource = source->GetShaderByName(shaderTypeToInstantiate->GetName());
    if (mainShaderTypeMergedToMergeFromSource == nullptr) return error(string("No shader exists in the source mixer with the name: ") + shaderTypeToInstantiate->GetName());

    //===================================================================================================================
    //===================================================================================================================
    //Get the list of all shaders to instantiate or merge
    vector<ShaderClassData*> listShader;
    vector<ShaderClassData*> shadersFullDependencies;
    listShader.push_back(mainShaderTypeMergedToMergeFromSource);
    if (!SpxStreamRemapper::GetShadersFullDependencies(source, listShader, shadersFullDependencies)) {
        source->copyMessagesTo(errorMessages);
        return error(string("Failed to get the shaders dependencies"));
    }

    vector<ShaderToMergeData> listShadersToMerge;
    for (auto its = shadersFullDependencies.begin(); its != shadersFullDependencies.end(); its++)
    {
        ShaderClassData* aShaderToMerge = *its;
        if (aShaderToMerge->dependencyType == ShaderClassData::ShaderDependencyTypeEnum::StaticFunctionCall)
        {
            //new merge: we won't add a shader if it already exists in the destination bytecode
            if (this->GetShaderByName(aShaderToMerge->GetName()) == nullptr)
            {
                listShadersToMerge.push_back(ShaderToMergeData(aShaderToMerge, false));
            }
        }
        else
        {
            //new instance
            listShadersToMerge.push_back(ShaderToMergeData(aShaderToMerge, true));
        }
    }

    //===================================================================================================================
    //===================================================================================================================
    //prefix to rename the shader, its variables and functions
    //Since we're using the shader name to identify them and retrieve them throughout mixin,
    //we need to make sure that the new instances will have a unique name and can't conflict with existing shader or previously created instances
    unsigned int prefixId = GetUniqueMergeOperationId();
    string namePrefix = string("o") + to_string(prefixId) + string("S") + to_string(compositionTarget->compositionShaderOwner->GetId()) + string("C") + to_string(compositionTargetId) + string("_");

    //Merge (duplicate) all shaders targeted by by the composition into the current bytecode
    if (!MergeShadersIntoBytecode(*source, listShadersToMerge, namePrefix))
    {
        error(string("failed to clone the shader for the composition: ") + compositionTarget->compositionShaderOwner->GetName() + string(".") + compositionTarget->variableName);
        return false;
    }
    ShaderClassData* mainShaderTypeMerged = mainShaderTypeMergedToMergeFromSource->tmpClonedShader;
    if (mainShaderTypeMerged == nullptr)
        return error(string("Merge shader function is expected to return a reference on the merged shader"));

    //===================================================================================================================
    //Map the list of targeted shaders with the cloned shader (so that we can update the links later on)
    /*unordered_map<spv::Id, spv::Id> mapCompositionShaderTargetedWithClonedShader;
    for (auto its = listAllShadersToInstantiate.begin(); its != listAllShadersToInstantiate.end(); its++)
    {
        ShaderClassData* shaderCloned = *its;
        ShaderClassData* clonedShader = shaderCloned->tmpClonedShader;
        mapCompositionShaderTargetedWithClonedShader[shaderCloned->GetId()] = clonedShader->GetId();
    }*/

    //===================================================================================================================
    //===================================================================================================================
    // - update our composition data, and add the instances instruction
    {
        //Analyse the function's bytecode, to add all new functions called
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        bool compositionUpdated = false;
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpShaderCompositionDeclaration:
                {
                    spv::Id shaderId = asId(start + 1);
                    int compositionId = asLiteralValue(start + 2);

                    if (shaderId == shaderCompositionTarget->GetId() && compositionId == compositionTargetId)
                    {
                        //increase number of instances
                        compositionTarget->countInstances++;
                        setLiteralValue(start + 5, compositionTarget->countInstances);

                        //Add the instance information in the bytecode
                        spv::Id mergedShaderTypeId = mainShaderTypeMerged->GetId();
                        spv::Instruction compInstanceInstr(spv::OpShaderCompositionInstance);
                        compInstanceInstr.addIdOperand(shaderId);
                        compInstanceInstr.addImmediateOperand(compositionId);
                        compInstanceInstr.addImmediateOperand(compositionTarget->countInstances - 1);
                        compInstanceInstr.addIdOperand(mergedShaderTypeId);

                        vector<unsigned int> instructionBytecode;
                        compInstanceInstr.dump(instructionBytecode);
                        spv.insert(spv.begin() + (start + wordCount), instructionBytecode.begin(), instructionBytecode.end());

                        compositionUpdated = true;
                        start = end;  //can stop here: there is only one composition to update
                    }
                    break;
                }
            }
            start += wordCount;
        }

        if (!compositionUpdated) return error("The composition has not been updated");

        if (!UpdateAllMaps()) return error("Failed to update all maps");
    }

    if (errorMessages.size() > 0) return false;
    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}

bool SpxStreamRemapper::MergeStreamMembers(TypeStructMemberArray& globalListOfMergedStreamVariables)
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
        {
            //first pass: get all stream variables by checking OpMemberProperties
            unsigned int start = header_size;
            const unsigned int end = spv.size();
            while (start < end)
            {
                unsigned int wordCount = asWordCount(start);
                spv::Op opCode = asOpCode(start);

                switch (opCode)
                {
                    case spv::OpMemberProperties:
                    {
                        poslastMemberXkslPropertiesEnd = start + wordCount;

                        const spv::Id typeId = asId(start + 1);
                        const unsigned int memberId = asLiteralValue(start + 2);
                        TypeInstruction* type = GetTypeById(typeId);
                        if (type == nullptr) { error(string("Cannot find the type for Id: ") + to_string(typeId)); break;}

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
                            typeStreamBufferData->members[memberId].structMemberId = memberId;
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
            const unsigned int end = spv.size();
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
                        if (memberId >= type->streamStructData->members.size()) {error("Invalid member id"); break;}
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

        //finally: get the stream members typeId  (define when creating the stream struct, for example: OpTypeStruct 22(int) 30(fvec4))
        if (success)
        {
            for (auto itt = listAllShaderTypeHoldingStreamVariables.begin(); itt != listAllShaderTypeHoldingStreamVariables.end(); itt++)
            {
                TypeInstruction* type = *itt;

                unsigned int start = type->bytecodeStartPosition;
                unsigned int wordCount = asWordCount(start);

                if (start > poslastTypeStreamDeclarationEnd) poslastTypeStreamDeclarationEnd = start + wordCount;

                spv::Op opCode = asOpCode(start);
                if (opCode != spv::OpTypeStruct) { error(string("Invalid OpCode for the stream struct type: ") + OpcodeString(opCode)); continue; }

                int countMembers = wordCount - 2;
                if (countMembers != type->streamStructData->members.size()) { error(string("Inconsistent number of members for the type: ") + type->GetName()); continue; }

                for (int m = 0; m < countMembers; ++m)
                {
                    int memberTypeId = asLiteralValue(start + 2 + m);
                    type->streamStructData->members[m].memberTypeId = memberTypeId;
                }
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
            if (type->streamStructData == nullptr) { error("streamStructData is null"); break;}
            for (auto itm = type->streamStructData->members.begin(); itm != type->streamStructData->members.end(); itm++)
            {
                const TypeStructMember& member = *itm;
                if (member.isStream == false || member.structMemberId == -1) { error("a steam member is invalid"); }
                if (member.HasSemantic() && member.isStage == false) { error("a steam member defines a semantic but is not a stage"); }
                if (member.memberTypeId == spv::spirvbin_t::unused) { error("a steam member has an undefined type Id"); }
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
    //regroup all stream variables in the same struct, merge the stage stream variables having the same semantic (or name, if no semantic is set)
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
                    aStreamMember.newStructMemberId = listOfMergedStreamVariables.size();
                    listOfMergedStreamVariables.push_back(aStreamMember);
                }
                else
                {
                    //refer to an existing stream
                    aStreamMember.newStructTypeId = streamStructTypeId;
                    aStreamMember.newStructMemberId = mergeWithStreamIndex;
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
    mapIndexesWithConstValueId.resize(listOfMergedStreamVariables.size(), spv::spirvbin_t::unused);
    spv::Id idOpTypeIntS32 = spv::spirvbin_t::unused;
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
                    if (idOpTypeIntS32 == spv::spirvbin_t::unused)
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
        if (idOpTypeIntS32 == spv::spirvbin_t::unused)
        {
            spv::Instruction typeInt32(newBoundId++, spv::NoType, spv::OpTypeInt);
            typeInt32.addImmediateOperand(32);
            typeInt32.addImmediateOperand(1);
            typeInt32.dump(vecConstsToAdd);
            idOpTypeIntS32 = typeInt32.getResultId();
        }
        for (unsigned int i = 0; i < mapIndexesWithConstValueId.size(); ++i)
        {
            if (mapIndexesWithConstValueId[i] == spv::spirvbin_t::unused)
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
        const unsigned int end = spv.size();
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
                        int newAccessChainIndex = streamBufferType->streamStructData->members[accessChainIndexValue32].newStructMemberId;
#ifdef XKSLANG_DEBUG_MODE
                        if (newAccessChainIndex < 0 || newAccessChainIndex >= (int)mapIndexesWithConstValueId.size()) { error("newAccessChainIndex is out of bound"); break; }
#endif
                        spv::Id newAccessChainIndexConstId = mapIndexesWithConstValueId[newAccessChainIndex];
#ifdef XKSLANG_DEBUG_MODE
                        if (newAccessChainIndexConstId == spv::spirvbin_t::unused) { error("invalid newAccessChainIndexConstId"); break; }
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

                //member name
                spv::Instruction memberName(spv::OpMemberName);
                memberName.addIdOperand(streamStructType.getResultId());
                memberName.addImmediateOperand(memberIndex);
                if (streamMember.HasSemantic()) {
                    memberName.addStringOperand((streamMember.semantic + (streamMember.isStage ? string("_s") : string("_")) + to_string(memberIndex)).c_str());
                }
                else {
                    memberName.addStringOperand((streamMember.declarationName + (streamMember.isStage ? string("_s") : string("_")) + to_string(memberIndex)).c_str());
                }
                memberName.dump(vecNamesAndDecorate);

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
        if (type->streamStructData != nullptr){
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

    //build the stream members struct info
    globalListOfMergedStreamVariables.members.clear();
    globalListOfMergedStreamVariables.structTypeId = streamStructTypeId;
    globalListOfMergedStreamVariables.structPointerTypeId = streamPointerStructTypeId;
    globalListOfMergedStreamVariables.structVariableTypeId = streamVarStructTypeId;
    for (unsigned int memberIndex = 0; memberIndex < listOfMergedStreamVariables.size(); ++memberIndex)
    {
        TypeStructMember& streamMember = listOfMergedStreamVariables[memberIndex];
        streamMember.structTypeId = streamMember.newStructTypeId;
        streamMember.structMemberId = streamMember.newStructMemberId;
        streamMember.newStructTypeId = 0;
        streamMember.newStructMemberId = 0;

        globalListOfMergedStreamVariables.members.push_back(streamMember);
    }
    globalListOfMergedStreamVariables.mapIndexesWithConstValueId = mapIndexesWithConstValueId;  //store for later use (so we won't have to refetch them)

    if (errorMessages.size() > 0) success = false;
    return success;
}

void SpxStreamRemapper::GetStagesPipeline(vector<ShadingStageEnum>& pipeline)
{
    pipeline = { ShadingStageEnum::Vertex, ShadingStageEnum::TessControl, ShadingStageEnum::TessEvaluation, ShadingStageEnum::Geometry , ShadingStageEnum::Pixel };
}

bool SpxStreamRemapper::InitializeCompilationProcess(vector<XkslMixerOutputStage>& outputStages)
{
    //if (status != SpxRemapperStatusEnum::MixinBeingCompiled_UnusedShaderRemoved) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_Initialized;

    if (outputStages.size() == 0) return error("no output stages defined");
    if (!ValidateHeader()) return error("Failed to validate the header");

#ifdef XKSLANG_DEBUG_MODE
    //check that the output stages are in the correct order
    vector<ShadingStageEnum> stagePipeline;
    SpxStreamRemapper::GetStagesPipeline(stagePipeline);

    int lastStageMatched = -1;
    for (unsigned int i = 0; i<outputStages.size(); ++i)
    {
        ShadingStageEnum outputStage = outputStages[i].outputStage->stage;

        lastStageMatched++;
        while (lastStageMatched < (int)stagePipeline.size())
        {
            if (stagePipeline[lastStageMatched] == outputStage) break;
            lastStageMatched++;
        }

        if (lastStageMatched == stagePipeline.size())
            return error(string("The output stage is unknown or not in the correct order: ") + GetShadingStageLabel(outputStage));
    }

#endif

    //===================================================================================================================
    //===================================================================================================================
    //For each output stages, we search the entryPoint function in the bytecode
    for (unsigned int i = 0; i<outputStages.size(); ++i)
    {
        FunctionInstruction* entryFunction = GetShaderFunctionForEntryPoint(outputStages[i].outputStage->entryPointName);
        if (entryFunction == nullptr) error(string("Entry point not found: ") + outputStages[i].outputStage->entryPointName);
        outputStages[i].entryFunction = entryFunction;
    }
    if (errorMessages.size() > 0) return false;

    return true;
}

bool SpxStreamRemapper::ValidateStagesStreamMembersFlow(vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_StreamsAnalysed) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_StreamReadyForReschuffling;

    if (outputStages.size() == 0) return error("no output stages defined");

    unsigned int countStreamMembers = globalListOfMergedStreamVariables.members.size();
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

        if (iStage == outputStages.size() - 1)
        {
            //pixel stage: defines the final outputs and the required inputs
            if (outputStage.outputStage->stage != ShadingStageEnum::Pixel) return error("Last output stage must be Pixel Stage");
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

bool SpxStreamRemapper::ReshuffleStreamVariables(vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_StreamReadyForReschuffling) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_StreamReschuffled;

    //get global stream buffer object
    if (globalListOfMergedStreamVariables.countMembers() == 0) return true; //nothing to reshuffle
    TypeInstruction* globalStreamType = GetTypeById(globalListOfMergedStreamVariables.structTypeId);
    if (globalStreamType == nullptr)
        return error(string("Cannot retrieve the global stream type. Id: ") + to_string(globalListOfMergedStreamVariables.structTypeId));

    //=============================================================================================================
    //=============================================================================================================
    //find the best positions where to insert new stuff
    int posNewTypesInsertion = globalStreamType->bytecodeStartPosition;
    int posNamesInsertion = header_size;
    {
        unsigned int start = header_size;
        const unsigned int end = spv.size();
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
    BytecodeChunk& bytecodeNewTypes = bytecodeUpdateController.InsertNewBytecodeChunckAt(posNewTypesInsertion);
    BytecodeChunk& bytecodeNames = bytecodeUpdateController.InsertNewBytecodeChunckAt(posNamesInsertion);
    spv::Id newId = bound();

    //=============================================================================================================
    //=============================================================================================================
    //when moving stream members from the global buffer to a function input/output/internal struct variable, we need to go through different pointer type (from "Private" to "Function")
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
        const unsigned int end = spv.size();
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
                memberPointerType.dump(bytecodeNewTypes.bytecode);
                mapStreamsMemberTypeIdToPointerFunctionTypeId[streamMember.memberTypeId] = memberPointerType.getResultId();
            }
            streamMember.memberPointerFunctionTypeId = mapStreamsMemberTypeIdToPointerFunctionTypeId[streamMember.memberTypeId];
        }
    }

    //=============================================================================================================
    //=============================================================================================================
    //The current stage's outputs structId will be used as inputs for the next stage
    spv::Id previousStageOutputStructTypeId = spv::spirvbin_t::unused;
    spv::Id previousStageOutputStructPointerTypeId = spv::spirvbin_t::unused;
    vector<unsigned int> previousStageVecOutputMembersIndex;

    //=============================================================================================================
    //Create all stream struct (input, output, internal stream) for each stages, updated all stream access, add all necessary data into the functions...
    for (unsigned int iStage = 0; iStage < outputStages.size(); ++iStage)
    {
        XkslMixerOutputStage& stage = outputStages[iStage];

#ifdef XKSLANG_DEBUG_MODE
        if (stage.listStreamVariablesAccessed.size() != globalListOfMergedStreamVariables.countMembers()) return error("invalid stream variable accessed list size");
#endif

        //===============================================================================
        //Find back which members are needed for which IOs
        vector<unsigned int> vecInputMembersIndex;
        vector<unsigned int> vecOutputMembersIndex;
        vector<unsigned int> vecIOMembersIndex;
        for (unsigned int index = 0; index < stage.listStreamVariablesAccessed.size(); ++index)
        {
            bool memberAdded = false;
            if (stage.listStreamVariablesAccessed[index].IsNeededAsInput()) {
                vecInputMembersIndex.push_back(index);
                vecIOMembersIndex.push_back(index);
                memberAdded = true;
            }
            if (stage.listStreamVariablesAccessed[index].IsNeededAsOutput()) {
                vecOutputMembersIndex.push_back(index);
                if (!memberAdded) vecIOMembersIndex.push_back(index);
                memberAdded = true;
            }

            if (memberAdded) globalListOfMergedStreamVariables.members[index].tmpRemapToIOIndex = vecIOMembersIndex.size() - 1;  //keep this info to remap the stream member to the corresponding IO
            else globalListOfMergedStreamVariables.members[index].tmpRemapToIOIndex = -1;
        }
        if (vecIOMembersIndex.size() == 0) continue;  //stage is not using any stream for input or output: no update needed for the stage

        //===============================================================================
        //Retrieve (and check validity of) the function declaration type
        FunctionInstruction* entryFunction = stage.entryFunction;

#ifdef XKSLANG_DEBUG_MODE
        if (entryFunction == nullptr) return error("Stage has no entry point function");
        spv::Op opCode = asOpCode(entryFunction->bytecodeStartPosition);
        if (opCode != spv::OpFunction) return error("Corrupted bytecode: expected OpFunction instruction");
#endif

        //function declaration type
        spv::Id functionDeclarationTypeId = asId(entryFunction->bytecodeStartPosition + 4);
        spv::Id functionReturnTypeId = asId(entryFunction->bytecodeStartPosition + 1);
        TypeInstruction* functionDeclarationType = GetTypeById(functionDeclarationTypeId);
        if (functionDeclarationType == nullptr) return error("Cannot find the function declaration type for Id: " + to_string(functionDeclarationTypeId));

#ifdef XKSLANG_DEBUG_MODE
        //some sanity check
        if (asOpCode(functionDeclarationType->bytecodeStartPosition) != spv::OpTypeFunction) return error("Corrupted bytecode: function declaration type must have OpTypeFunction instruction");
        if (functionReturnTypeId != asId(functionDeclarationType->bytecodeStartPosition + 2)) return error("Corrupted bytecode: function return type must be the same as function declaration's return type");
#endif

        //confirm that the function's return type is void
        TypeInstruction* initialReturnType = GetTypeById(functionReturnTypeId);
        if (initialReturnType == nullptr) return error("Cannot find the function return type for Id: " + to_string(functionReturnTypeId));
        if (asOpCode(initialReturnType->bytecodeStartPosition) != spv::OpTypeVoid) return error(GetShadingStageLabel(stage.outputStage->stage) + " stage's entry function must initially return void");

        //===============================================================================
        spv::Id inputStructTypeId = spv::spirvbin_t::unused;
        spv::Id inputStructPointerTypeId = spv::spirvbin_t::unused;
        spv::Id outputStructTypeId = spv::spirvbin_t::unused;
        spv::Id outputStructPointerTypeId = spv::spirvbin_t::unused;
        spv::Id streamStructTypeId = spv::spirvbin_t::unused;
        spv::Id streamStructPointerTypeId = spv::spirvbin_t::unused;
        spv::Id streamStructFunctionVariableId = spv::spirvbin_t::unused;
        spv::Id newFunctionDeclarationTypeId = spv::spirvbin_t::unused;
        spv::Id functionInputParameterResultId = spv::spirvbin_t::unused;

        //===============================================================================
        //create the function's input structure
        if (iStage == 0)
        {
            if (vecInputMembersIndex.size() > 0)
            {
                //make the struct type
                spv::Instruction structType(newId++, spv::NoType, spv::OpTypeStruct);
                for (unsigned int k = 0; k < vecInputMembersIndex.size(); ++k)
                {
                    const unsigned int index = vecInputMembersIndex[k];
                    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                    structType.addIdOperand(streamMember.memberTypeId);
                }
                structType.dump(bytecodeNewTypes.bytecode);

                //make the struct function parameter pointer type
                spv::Instruction structPointerType(newId++, spv::NoType, spv::OpTypePointer);
                structPointerType.addImmediateOperand(spv::StorageClass::StorageClassFunction);
                structPointerType.addIdOperand(structType.getResultId());
                structPointerType.dump(bytecodeNewTypes.bytecode);

                inputStructTypeId = structType.getResultId();
                inputStructPointerTypeId = structPointerType.getResultId();

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                //struct name
                string stagePrefix = GetShadingStageLabelShort(stage.outputStage->stage) + "_IN";
                spv::Instruction structName(spv::OpName);
                structName.addIdOperand(structType.getResultId());
                structName.addStringOperand(stagePrefix.c_str());
                structName.dump(bytecodeNames.bytecode);

                //Add the members name
                for (unsigned int k = 0; k < vecInputMembersIndex.size(); ++k)
                {
                    const unsigned int index = vecInputMembersIndex[k];
                    const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                    //member name
                    spv::Instruction memberName(spv::OpMemberName);
                    memberName.addIdOperand(structType.getResultId());
                    memberName.addImmediateOperand(k);
                    memberName.addStringOperand((streamMember.GetSemanticOrDeclarationName() + "_" + to_string(k)).c_str());
                    memberName.dump(bytecodeNames.bytecode);
                }
#endif
            }
        }
        else
        {
            //For the current stage's input structs, we can use the previous stage's output struct
            inputStructTypeId = previousStageOutputStructTypeId;
            inputStructPointerTypeId = previousStageOutputStructPointerTypeId;

            //just double check that the previous stage output stream variables match the current stage's input stream variable
            if (previousStageVecOutputMembersIndex.size() != vecInputMembersIndex.size())
                return error("previous stage output streams does not match the current stage's input streams");
            for (unsigned int k = 0; k < previousStageVecOutputMembersIndex.size(); ++k)
            {
                if (previousStageVecOutputMembersIndex[k] != vecInputMembersIndex[k])
                    return error("previous stage output streams does not match the current stage's input streams");
            }
        }

        //===============================================================================
        //create the function's output structure
        if (vecOutputMembersIndex.size() > 0)
        {
            //make the struct type
            spv::Instruction structType(newId++, spv::NoType, spv::OpTypeStruct);
            for (unsigned int k = 0; k < vecOutputMembersIndex.size(); ++k)
            {
                const unsigned int index = vecOutputMembersIndex[k];
                const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                structType.addIdOperand(streamMember.memberTypeId);
            }
            structType.dump(bytecodeNewTypes.bytecode);

            //make the struct function pointer type
            spv::Instruction structPointerType(newId++, spv::NoType, spv::OpTypePointer);
            structPointerType.addImmediateOperand(spv::StorageClass::StorageClassFunction);
            structPointerType.addIdOperand(structType.getResultId());
            structPointerType.dump(bytecodeNewTypes.bytecode);

            outputStructTypeId = structType.getResultId();
            outputStructPointerTypeId = structPointerType.getResultId();

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //struct name
            string stagePrefix = GetShadingStageLabelShort(stage.outputStage->stage) + "_OUT";
            spv::Instruction structName(spv::OpName);
            structName.addIdOperand(structType.getResultId());
            structName.addStringOperand(stagePrefix.c_str());
            structName.dump(bytecodeNames.bytecode);

            //Add the members name
            for (unsigned int k = 0; k < vecOutputMembersIndex.size(); ++k)
            {
                const unsigned int index = vecOutputMembersIndex[k];
                const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                //member name
                spv::Instruction memberName(spv::OpMemberName);
                memberName.addIdOperand(structType.getResultId());
                memberName.addImmediateOperand(k);
                memberName.addStringOperand((streamMember.GetSemanticOrDeclarationName() + "_" + to_string(k)).c_str());
                memberName.dump(bytecodeNames.bytecode);
            }
#endif
        }

        //===============================================================================
        //create the function's stream IO structure
        {
            //make the struct type
            spv::Instruction structType(newId++, spv::NoType, spv::OpTypeStruct);
            for (unsigned int k = 0; k < vecIOMembersIndex.size(); ++k)
            {
                const unsigned int index = vecIOMembersIndex[k];
                const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                structType.addIdOperand(streamMember.memberTypeId);
            }
            structType.dump(bytecodeNewTypes.bytecode);

            //make the struct function pointer type
            spv::Instruction structPointerType(newId++, spv::NoType, spv::OpTypePointer);
            structPointerType.addImmediateOperand(spv::StorageClass::StorageClassFunction);
            structPointerType.addIdOperand(structType.getResultId());
            structPointerType.dump(bytecodeNewTypes.bytecode);

            streamStructTypeId = structType.getResultId();
            streamStructPointerTypeId = structPointerType.getResultId();

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //struct name
            string stagePrefix = GetShadingStageLabelShort(stage.outputStage->stage) + "_STREAMS";
            spv::Instruction structName(spv::OpName);
            structName.addIdOperand(structType.getResultId());
            structName.addStringOperand(stagePrefix.c_str());
            structName.dump(bytecodeNames.bytecode);

            //Add the members name
            for (unsigned int k = 0; k < vecIOMembersIndex.size(); ++k)
            {
                const unsigned int index = vecIOMembersIndex[k];
                const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];

                //member name
                spv::Instruction memberName(spv::OpMemberName);
                memberName.addIdOperand(structType.getResultId());
                memberName.addImmediateOperand(k);
                memberName.addStringOperand((streamMember.GetSemanticOrDeclarationName() + "_" + to_string(k)).c_str());
                memberName.dump(bytecodeNames.bytecode);
            }
#endif
        }

        //===============================================================================
        //Create the new function type (we don't update the current one, in the case of it was used by several function)
        {
            spv::Instruction functionNewTypeInstruction(newId++, spv::NoType, spv::OpTypeFunction);
            newFunctionDeclarationTypeId = functionNewTypeInstruction.getResultId();

            //return type
            if (outputStructTypeId != spv::spirvbin_t::unused) functionNewTypeInstruction.addIdOperand(outputStructTypeId);
            else functionNewTypeInstruction.addIdOperand(functionReturnTypeId);  //original return type (void)

            //input parameters
            if (inputStructPointerTypeId != spv::spirvbin_t::unused) functionNewTypeInstruction.addIdOperand(inputStructPointerTypeId);  //new input struct type (if any)
            for (unsigned int pos = functionDeclarationType->bytecodeStartPosition + 3; pos < functionDeclarationType->bytecodeEndPosition; pos++)
                functionNewTypeInstruction.addIdOperand(asId(pos)); //add all previous input parameters

            functionNewTypeInstruction.dump(bytecodeNewTypes.bytecode);
        }
        
        //===============================================================================
        //Add the input struct into the function parameter list
        if (inputStructPointerTypeId != spv::spirvbin_t::unused)
        {
            unsigned int wordCount = asWordCount(entryFunction->bytecodeStartPosition);
            int entryFunctionFirstInstruction = entryFunction->bytecodeStartPosition + wordCount;
            BytecodeChunk& bytecodeFunctionParameterChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(entryFunctionFirstInstruction);
            spv::Instruction functionInputParameterInstr(newId++, inputStructPointerTypeId, spv::OpFunctionParameter);
            functionInputParameterInstr.dump(bytecodeFunctionParameterChunk.bytecode);
            functionInputParameterResultId = functionInputParameterInstr.getResultId();

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //param name
            spv::Instruction stageInputStructName(spv::OpName);
            stageInputStructName.addIdOperand(functionInputParameterInstr.getResultId());
            stageInputStructName.addStringOperand(functionInputVariableDefaultName.c_str());
            stageInputStructName.dump(bytecodeNames.bytecode);
#endif
        }

        //===============================================================================
        // Add variables (stream, output) into the function, and initialize them
        {
            //Get the function starting and ending positions
            unsigned int functionLabelInstrPos;
            unsigned int functionReturnInstrPos;

            if (!GetFunctionLabelAndReturnInstructionsPosition(entryFunction, functionLabelInstrPos, functionReturnInstrPos))
                return error("Failed to parse the function: " + entryFunction->GetFullName());
            unsigned int functionLabelInstrEndPos = functionLabelInstrPos + asWordCount(functionLabelInstrPos); //go at the end of the label instruction
            if (asOpCode(functionReturnInstrPos) != spv::OpReturn) return error("Invalid function end. Expecting OpReturn instructions.");  //could be another return type opInstruction

            //Add the stream variable into the function variables list
            BytecodeChunk& functionStartingInstructionsChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionLabelInstrEndPos);
            spv::Instruction functionStreamVariable(newId++, streamStructPointerTypeId, spv::OpVariable);
            functionStreamVariable.addImmediateOperand(spv::StorageClassFunction);
            functionStreamVariable.dump(functionStartingInstructionsChunk.bytecode);
            streamStructFunctionVariableId = functionStreamVariable.getResultId();

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
            //stream variable name
            spv::Instruction outputVariableName(spv::OpName);
            outputVariableName.addIdOperand(functionStreamVariable.getResultId());
            outputVariableName.addStringOperand(functionStreamVariableDefaultName.c_str());
            outputVariableName.dump(bytecodeNames.bytecode);
#endif

            //Add the output variables (if any) into the function variables list
            if (outputStructTypeId != spv::spirvbin_t::unused)
            {
                spv::Instruction functionOutputVariable(newId++, outputStructPointerTypeId, spv::OpVariable);
                functionOutputVariable.addImmediateOperand(spv::StorageClassFunction);
                functionOutputVariable.dump(functionStartingInstructionsChunk.bytecode);

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
                //output variable name
                spv::Instruction outputVariableName(spv::OpName);
                outputVariableName.addIdOperand(functionOutputVariable.getResultId());
                outputVariableName.addStringOperand(functionOutputVariableDefaultName.c_str());
                outputVariableName.dump(bytecodeNames.bytecode);
#endif

                //update the function return type (must be equal to the function declaration type's return type)
                bytecodeUpdateController.SetNewAtomicValueUpdate(entryFunction->bytecodeStartPosition + 1, outputStructTypeId);

                //add instruction to load the output (and replace OpReturn by OpReturnValue)
                BytecodeChunk& functionFinalInstructionsChunk = bytecodeUpdateController.InsertNewBytecodeChunckAt(functionReturnInstrPos, 1);  //1 to tell that we will overlap 1 byte
                spv::Instruction loadingOutputInstruction(newId++, outputStructTypeId, spv::OpLoad);
                loadingOutputInstruction.addIdOperand(functionOutputVariable.getResultId());
                loadingOutputInstruction.dump(functionFinalInstructionsChunk.bytecode);

                //change OpReturn instruction to OpReturnValue 
                spv::Instruction returnValueInstruction(spv::NoResult, spv::NoType, spv::OpReturnValue);
                returnValueInstruction.addIdOperand(loadingOutputInstruction.getResultId());
                returnValueInstruction.dump(functionFinalInstructionsChunk.bytecode);
            }

            //===============================================================================
            //copy the input streams (if any) into the stage stream struct
            if (vecInputMembersIndex.size() > 0)
            {
#ifdef XKSLANG_DEBUG_MODE
                if (functionInputParameterResultId == spv::spirvbin_t::unused) return error("Invalid functionInputParameterResultId");
#endif
                for (unsigned int k = 0; k < vecInputMembersIndex.size(); ++k)
                {
                    //TOTOTOTOTOTOTO

                    ///  int accessConstTypeId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[k];
                    ///  spv::Instruction accessInputStreamInstr(newId++, ptrId, spv::OpAccessChain);
                    ///  accessInputStreamInstr.addIdOperand(functionInputParameterResultId);
                    ///  accessInputStreamInstr.addImmediateOperand(accessConstTypeId);
                    ///  accessInputStreamInstr.dump(functionStartingInstructionsChunk.bytecode);

                    //const unsigned int index = vecInputMembersIndex[k];
                    //const TypeStructMember& streamMember = globalListOfMergedStreamVariables.members[index];
                    //structType.addIdOperand(streamMember.memberTypeId);
                }
            }
        }

        //===============================================================================
        //update the function declaration type to refer to new one
        if (newFunctionDeclarationTypeId != spv::spirvbin_t::unused)
        {
            bytecodeUpdateController.SetNewAtomicValueUpdate(entryFunction->bytecodeStartPosition + 4, newFunctionDeclarationTypeId);
        }
        else return error("No new function type must be created");

        //===============================================================================
        //Current stage's output data will be used by the next stage input data
        previousStageOutputStructTypeId = outputStructTypeId;
        previousStageOutputStructPointerTypeId = outputStructPointerTypeId;
        previousStageVecOutputMembersIndex = vecOutputMembersIndex;

        //===============================================================================
        //All input, output and stream structs have been created, we can parse the function bytecode and update the reference to all streams members
        {
            unsigned int start = entryFunction->bytecodeStartPosition;
            const unsigned int end = entryFunction->bytecodeEndPosition;
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
                            if (globalStreamMemberIndex < 0 || globalStreamMemberIndex >= (int)globalListOfMergedStreamVariables.members.size())
                                return error(string("globalStreamMemberIndex is out of bound: ") + to_string(globalStreamMemberIndex));
                            if (globalListOfMergedStreamVariables.members[globalStreamMemberIndex].tmpRemapToIOIndex == -1)
                                return error("The global stream member has not been selected as IO stage stream");
#endif
                            int ioStreamMemberIndex = globalListOfMergedStreamVariables.members[globalStreamMemberIndex].tmpRemapToIOIndex; //new index for the members in the IO stream
#ifdef XKSLANG_DEBUG_MODE
                            if (ioStreamMemberIndex < 0 || ioStreamMemberIndex >= (int)vecIOMembersIndex.size())
                                return error(string("ioStreamMemberIndex is out of bound: ") + to_string(ioStreamMemberIndex));
#endif
                            spv::Id ioMemberConstId = globalListOfMergedStreamVariables.mapIndexesWithConstValueId[ioStreamMemberIndex]; //get the const type id for new index

                            bytecodeUpdateController.SetNewAtomicValueUpdate(start + 3, streamStructFunctionVariableId);
                            bytecodeUpdateController.SetNewAtomicValueUpdate(start + 4, ioMemberConstId);

                            //We change the OpAccessChain's pointertype (typeId) from private to function
                            spv::Id memberPointerFunctionTypeId = globalListOfMergedStreamVariables.members[globalStreamMemberIndex].memberPointerFunctionTypeId;
#ifdef XKSLANG_DEBUG_MODE
                            if (memberPointerFunctionTypeId <= 0) return error(string("memberPointerFunctionTypeId has not be found or created"));
#endif
                            bytecodeUpdateController.SetNewAtomicValueUpdate(start + 1, memberPointerFunctionTypeId);
                        }
                        break;
                    }
                }
                start += wordCount;
            }
        }

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

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::ApplyBytecodeUpdateController(const BytecodeUpdateController& bytecodeUpdateController)
{
    unsigned int bytecodeOriginalSize = spv.size();

    //first : update all values    
    for (auto itau = bytecodeUpdateController.listAtomicUpdates.begin(); itau != bytecodeUpdateController.listAtomicUpdates.end(); itau++)
    {
        const BytecodeAtomicValueUpdate& atomicValueUpdate = *itau;

#ifdef XKSLANG_DEBUG_MODE
        if (atomicValueUpdate.pos >= bytecodeOriginalSize) {error("pos is out of bound"); break;}
#endif

        spv[atomicValueUpdate.pos] = atomicValueUpdate.value;
    }

    //then Insert all new chuncks in the bytecode
    for (auto itbc = bytecodeUpdateController.listSortedChunksToInsert.begin(); itbc != bytecodeUpdateController.listSortedChunksToInsert.end(); itbc++)
    {
        const BytecodeChunk& bytecodeChunck = *itbc;

#ifdef XKSLANG_DEBUG_MODE
        if (bytecodeChunck.insertionPos > bytecodeOriginalSize) { error("bytecode chunck is out of bound"); break; }
        if (bytecodeChunck.countInstructionsToOverlap > bytecodeChunck.bytecode.size()) { error("bytecode chunck overlaps too many bytes"); break; }
#endif

        if (bytecodeChunck.countInstructionsToOverlap == 0)
        {
            spv.insert(spv.begin() + bytecodeChunck.insertionPos, bytecodeChunck.bytecode.begin(), bytecodeChunck.bytecode.end());
        }
        else
        {
            //We overlap some bytes, and maybe add some more
            int countOverlaps = bytecodeChunck.countInstructionsToOverlap;
            int countRemaining = bytecodeChunck.bytecode.size() - countOverlaps;
            for (unsigned int i = 0; i < countOverlaps; ++i) spv[bytecodeChunck.insertionPos + i] = bytecodeChunck.bytecode[i];
            if (countRemaining > 0)
                spv.insert(spv.begin() + (bytecodeChunck.insertionPos + countOverlaps), bytecodeChunck.bytecode.begin() + countOverlaps, bytecodeChunck.bytecode.end());
        }
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::AnalyseStreamMembersUsageForOutputStages(vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables)
{
    //if (status != SpxRemapperStatusEnum::MixinBeingCompiled_UnusedShaderRemoved) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_StreamsAnalysed;

    if (globalListOfMergedStreamVariables.countMembers() == 0) return true; //nothing to analyse
    spv::Id globalStreamStructVariableId = globalListOfMergedStreamVariables.structVariableTypeId;

    //Set all functions stage reserve value to undefine
    for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
        FunctionInstruction* aFunction = *itsf;
        aFunction->stageReservingTheFunction = ShadingStageEnum::Undefined;
    }

    //Analyse each stage
    for (unsigned int iStage = 0; iStage < outputStages.size(); iStage++)
    {
        XkslMixerOutputStage* outputStage = &(outputStages[iStage]);
        if (outputStage->entryFunction == nullptr) return error("A stage entry point function is null.");

        //reset output stage data
        outputStage->listCalledFunctionsAccessingStreamMembers.clear();
        outputStage->listStreamVariablesAccessed.clear();
        for (unsigned int m = 0; m<globalListOfMergedStreamVariables.members.size(); ++m)
            outputStage->listStreamVariablesAccessed.push_back(MemberAccessDetails());

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
                            spv::Id indexConstId= asId(start + 4);

                            ConstInstruction* constObject = GetConstById(indexConstId);
                            if (constObject == nullptr) return error(string("cannot get const object for Id: ") + to_string(indexConstId));
                            int streamMemberIndex = constObject->valueS32;
#ifdef XKSLANG_DEBUG_MODE
                            if (resultId >= vectorResultIdsAccessingAStreamVariable.size()) return error("resultId out of bound");
                            if (vectorResultIdsAccessingAStreamVariable[resultId] != -1) return error("resultId is already accessing a stream variable");
                            if (streamMemberIndex < 0 || streamMemberIndex >= (int)globalListOfMergedStreamVariables.members.size())
                                return error(string("streamMemberIndex is out of bound: ") + to_string(streamMemberIndex));
#endif
                            vectorResultIdsAccessingAStreamVariable[resultId] = streamMemberIndex;
                            isFunctionAccessingAStreamVariable = true;
                        }
                        break;
                    }

                    case spv::OpFunctionCall:
                    case spv::OpFunctionCallBaseResolved:
                    {
                        //pile the function to go check it later
                        spv::Id functionCalledId = asId(start + 3);
                        FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                        if (anotherFunctionCalled->flag1 == 0){
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
                if (aFunctionCalled->stageReservingTheFunction != ShadingStageEnum::Undefined)
                {
                    return error(GetShadingStageLabel(aFunctionCalled->stageReservingTheFunction) + " and " + GetShadingStageLabel(outputStage->outputStage->stage)
                         + " stages are both calling a function accessing stream members. Function name: " + aFunctionCalled->GetFullName() );
                }

                aFunctionCalled->stageReservingTheFunction = outputStage->outputStage->stage;
                outputStage->listCalledFunctionsAccessingStreamMembers.push_back(aFunctionCalled);
            }
        }

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
                        spv::Id targetId = (opCode == spv::OpStore)? asId(start + 1) : asId(start + 3);
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
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::RemoveAllUnusedShaders(vector<XkslMixerOutputStage>& outputStages)
{
    //if (status != SpxRemapperStatusEnum::MixinBeingCompiled_UnusedShaderRemoved) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_UnusedShaderRemoved;

    if (outputStages.size() == 0) return error("no output stages defined");
    if (!ValidateHeader()) return error("Failed to validate the header");

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
        for (unsigned int i = 0; i<outputStages.size(); ++i)
        {
            if (outputStages[i].entryFunction == nullptr) return error("An entry point function is null.");
            vectorFunctionsCalled.push_back(outputStages[i].entryFunction);
        }

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
                        if (anotherFunctionCalled->flag1 == 0) vectorFunctionsCalled.push_back(anotherFunctionCalled); //we'll analyse the function later
                        break;
                    }

                    case spv::OpFunctionCallBaseUnresolved:
                    {
                        error(string("An unresolved function base call has been found in function: ") + aFunctionCalled->GetFullName());
                        break;
                    }

                    case spv::OpFunctionCallThroughCompositionVariable:
                    {
                        error(string("An unresolved function call through composition has been found in function: ") + aFunctionCalled->GetFullName());
                        break;
                    }
                }
                start += wordCount;
            }
        }
        if (errorMessages.size() > 0) return false;

        //====================================================================
        //For all shader flagged, flag their whole family (composition instances and parents) as well
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

            //add the composition shader instances
            for (auto itc = aShaderInvolved->compositionsList.begin(); itc != aShaderInvolved->compositionsList.end(); itc++)
            {
                const ShaderComposition& aComposition = *itc;
                if (aComposition.countInstances > 0)
                {
                    vector<ShaderClassData*> vecCompositionShaderInstances;
                    if (!GetAllShaderInstancesForComposition(&aComposition, vecCompositionShaderInstances)) {
                        error(string("Failed to retrieve the instances for the composition: ") + aComposition.variableName + string(" from shader: ") + aComposition.compositionShaderOwner->GetName());
                    }
                    else
                    {
                        for (unsigned int ii = 0; ii < vecCompositionShaderInstances.size(); ii++)
                        {
                            ShaderClassData* aShaderCompositionInstanced = vecCompositionShaderInstances[ii];
                            if (aShaderCompositionInstanced->flag1 != 2) vectorShadersOwningAllFunctionsCalled.push_back(aShaderCompositionInstanced);
                        }
                    }
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
            if (!RemoveShaderFromBytecodeAndData(unusedShader, vecStripRanges))
            {
                return error(string("Failed to remove the unused shader: ") + unusedShader->GetName());
            }
        }

        stripBytecode(vecStripRanges);
        if (!UpdateAllMaps()) return error("failed to update all maps");
    }

    return true;
}

bool SpxStreamRemapper::ApplyCompositionInstancesToBytecode()
{
    //if (status != SpxRemapperStatusEnum::MixinBeingCompiled_UnusedShaderRemoved) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_CompositionInstancesProcessed;

    //===================================================================================================================
    //===================================================================================================================
    //Duplicate all foreach loops depending on the composition instances

    //======================================================================================
    //Build the list of all foreach loops, plus the list of all composition instances data
    int maxForEachLoopsNestedLevel;
    vector<CompositionForEachLoopData> vecAllForeachLoops;
    if (!GetAllCompositionForEachLoops(vecAllForeachLoops, maxForEachLoopsNestedLevel)) {
        return error(string("Failed to retrieve all composition foreach loops for the composition: "));
    }

    //======================================================================================
    //duplicate the foreach loops for all instances
    {
        for (int nestedLevelToProcess = maxForEachLoopsNestedLevel; nestedLevelToProcess >= 0; nestedLevelToProcess--)
        {
            list<CompositionForEachLoopData*> listForeachLoopsToMergeIntoBytecode;

            int maxId = bound();
            vector<bool> IdsToRemapTable;
            IdsToRemapTable.resize(bound(), false);

            for (auto itfe = vecAllForeachLoops.begin(); itfe != vecAllForeachLoops.end(); itfe++)
            {
                CompositionForEachLoopData* forEachLoopToUnroll = &(*itfe);
                if (forEachLoopToUnroll->nestedLevel != nestedLevelToProcess) continue;

                ShaderComposition* compositionToUnroll = forEachLoopToUnroll->composition;
                
                bool anythingToUnroll = true;
                if (compositionToUnroll->countInstances <= 0) anythingToUnroll = false;  //if no instances set, nothing to unroll
                if (forEachLoopToUnroll->firstLoopInstuctionStart >= forEachLoopToUnroll->lastLoopInstuctionEnd) anythingToUnroll = false;  //if no bytecode within the forloop, nothing to unroll

                if (anythingToUnroll)
                {
                    vector<uint32_t> duplicatedBytecode;
                    vector<uint32_t> foreachLoopBytecode;
                    foreachLoopBytecode.insert(foreachLoopBytecode.end(), spv.begin() + forEachLoopToUnroll->firstLoopInstuctionStart, spv.begin() + forEachLoopToUnroll->lastLoopInstuctionEnd);

                    //get the list of all resultIds to remapp from the foreach loop bytecode
                    {
                        unsigned int start = 0;
                        const unsigned int end = foreachLoopBytecode.size();
                        while (start < end)
                        {
                            spv::Op opCode = spirvbin_t::opOpCode(foreachLoopBytecode[start]);
                            unsigned int wordCount = spirvbin_t::opWordCount(foreachLoopBytecode[start]);
                            unsigned int word = start + 1;
                        
                            // any type?
                            if (spv::InstructionDesc[opCode].hasType()) {
                                word++;
                            }

                            // any result id to remap?
                            if (spv::InstructionDesc[opCode].hasResult()) {
                                spv::Id resultId = foreachLoopBytecode[word];
#ifdef XKSLANG_DEBUG_MODE
                                if (resultId < 0 || resultId >= IdsToRemapTable.size())
                                { error(string("unroll foreach composition: result id is out of bound. Id: ") + to_string(resultId)); break; }
#endif
                                IdsToRemapTable[resultId] = true;
                            }

                            start += wordCount;
                        }
                    }

                    //Get all instances set for the composition
                    vector<ShaderClassData*> vecCompositionShaderInstances;
                    if (!GetAllShaderInstancesForComposition(compositionToUnroll, vecCompositionShaderInstances)) {
                        return error(string("Failed to retrieve the instances for the composition: ") + compositionToUnroll->variableName + string(" from shader: ") + compositionToUnroll->compositionShaderOwner->GetName());
                    }

                    //duplicate the foreach loop bytecode for all instances (compositions are already sorted by their num)
                    unsigned int countInstances = vecCompositionShaderInstances.size();
                    for (unsigned int instanceNum = 0; instanceNum < countInstances; ++instanceNum)
                    {
                        ShaderClassData* compositionShaderInstance = vecCompositionShaderInstances[instanceNum];

                        //Clone the loop bytecode at the end of the duplicated bytecode
                        {
                            vector<spv::Id> remapTable;
                            remapTable.resize(bound(), spv::spirvbin_t::unused);
                            for (unsigned int id=0; id<IdsToRemapTable.size(); ++id) {
                                if (IdsToRemapTable[id]) remapTable[id] = maxId++;
                                else remapTable[id] = id;
                            }

                            unsigned int start = duplicatedBytecode.size();
                            duplicatedBytecode.insert(duplicatedBytecode.end(), foreachLoopBytecode.begin(), foreachLoopBytecode.end());
                            const unsigned int end = duplicatedBytecode.size();

                            //remap all Ids
                            if (!remapAllIds(duplicatedBytecode, start, end, remapTable))
                                return error("remapAllIds failed on duplicatedBytecode");

                            //for all call to a function through the composition variable we're instancing, update the instance num
                            while (start < end)
                            {
                                unsigned int wordCount = opWordCount(duplicatedBytecode[start]);
                                spv::Op opCode = opOpCode(duplicatedBytecode[start]);
                                switch (opCode)
                                {
                                    case spv::OpFunctionCallThroughCompositionVariable:
                                    {
                                        spv::Id shaderId = duplicatedBytecode[start + 4];
                                        int compositionId = duplicatedBytecode[start + 5];
                                        int initialInstanceNum = duplicatedBytecode[start + 6];

                                        if (compositionToUnroll->compositionShaderOwner->GetId() == shaderId && compositionToUnroll->compositionShaderId == compositionId)
                                        {
                                            duplicatedBytecode[start + 6] = instanceNum;
                                        }

                                        break;
                                    }
                                }
                                start += wordCount;
                            }
                        }
                    }

                    forEachLoopToUnroll->foreachDuplicatedBytecode = duplicatedBytecode;

                    //insert the foreach loop, sorted by their reversed apparition in the bytecode
                    auto itList = listForeachLoopsToMergeIntoBytecode.begin();
                    while (itList != listForeachLoopsToMergeIntoBytecode.end())
                    {
                        if (forEachLoopToUnroll->foreachLoopStart > (*itList)->foreachLoopStart) break;
                    }
                    listForeachLoopsToMergeIntoBytecode.insert(itList, forEachLoopToUnroll);

                }  //end of if (anythingToUnroll)
            } //for (auto itfe = vecAllForeachLoops.begin(); itfe != vecAllForeachLoops.end(); itfe++)

            //we finished unrolling all foreach loops for the nested level, we can apply them to the bytecode
            unsigned int insertionPos = spv.size();
            for (auto itfe = listForeachLoopsToMergeIntoBytecode.begin(); itfe != listForeachLoopsToMergeIntoBytecode.end(); itfe++)
            {
                CompositionForEachLoopData* forEachLoopToMerge = *itfe;
                if (forEachLoopToMerge->foreachLoopEnd > insertionPos) return error("foreach loops are not sorted correctly");
                insertionPos = forEachLoopToMerge->foreachLoopEnd;
                
                spv.insert(spv.begin() + insertionPos, forEachLoopToMerge->foreachDuplicatedBytecode.begin(), forEachLoopToMerge->foreachDuplicatedBytecode.end());
            }

            //we updated the bytecode, so refetch all foreach loops
            setBound(maxId);
            if (!GetAllCompositionForEachLoops(vecAllForeachLoops, maxForEachLoopsNestedLevel)) {
                return error(string("Failed to retrieve all composition foreach loops for the composition: "));
            }

        } //end for (int nestedLevelToProcess = maxForEachLoopsNestedLevel; nestedLevelToProcess >= 0; nestedLevelToProcess--)
    } 

    //======================================================================================
    //remove all foreach loops
    if (vecAllForeachLoops.size() > 0)
    {
        vector<range_t> vecStripRanges;
        for (auto itfe = vecAllForeachLoops.begin(); itfe != vecAllForeachLoops.end(); itfe++)
        {
            const CompositionForEachLoopData& forEachLoop = *itfe;
            if (forEachLoop.nestedLevel == 0)  //nested level > 0 are included inside level 0
            {
                vecStripRanges.push_back(range_t(forEachLoop.foreachLoopStart, forEachLoop.foreachLoopEnd));
            }
        }

        stripBytecode(vecStripRanges);
        if (errorMessages.size() > 0) return error("ApplyAllCompositions: failed to remove the foreach loops");
        //Update all maps (update objects position in the bytecode)
        if (!UpdateAllMaps()) return error("ApplyAllCompositions: failed to update all maps");
    }

    //===================================================================================================================
    //===================================================================================================================
    //updates all OpFunctionCallThroughCompositionVariable instructions calling through the composition that we're instancing   
    {
        vector<range_t> vecStripRanges;
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpFunctionCallThroughCompositionVariable:
                {
                    spv::Id shaderId = asId(start + 4);
                    int compositionId = asLiteralValue(start + 5);
                    unsigned int instancesNum = asLiteralValue(start + 6);

                    //===================================================
                    //Find the composition data
                    ShaderClassData* compositionShaderOwner = this->GetShaderById(shaderId);
                    if (compositionShaderOwner == nullptr) { error(string("No shader exists with the Id: ") + to_string(shaderId)); break; }

                    ShaderComposition* composition = compositionShaderOwner->GetShaderCompositionById(compositionId);
                    if (composition == nullptr) { error(string("Shader: ") + compositionShaderOwner->GetName() + string(" has no composition for id: ") + to_string(compositionId)); break; }

                    // If an OpFunctionCallThroughCompositionVariable instructions has no composition instance we ignore it
                    // (it won't generate an error as long as the instruction is not called by any compilation output functions)
                    if (composition->countInstances == 0) break;

                    vector<ShaderClassData*> vecCompositionShaderInstances;
                    if (!GetAllShaderInstancesForComposition(composition, vecCompositionShaderInstances)) {
                        return error(string("Failed to retrieve the instances for the composition: ") + composition->variableName + string(" from shader: ") + composition->compositionShaderOwner->GetName());
                    }

                    if (instancesNum >= vecCompositionShaderInstances.size()) { error(string("Invalid instanceNum number: ") + to_string(instancesNum)); break; }
                    ShaderClassData* clonedShader = vecCompositionShaderInstances[instancesNum];

                    //===================================================
                    //Get the original function called
                    spv::Id originalFunctionId = asId(start + 3);
                    FunctionInstruction* functionToReplace = GetFunctionById(originalFunctionId);
                    if (functionToReplace == nullptr) {
                        error(string("OpFunctionCallThroughCompositionVariable: targeted Id is not a known function. Id: ") + to_string(originalFunctionId));
                        return true;
                    }

                    //We retrieve the cloned function using the function name
                    FunctionInstruction* functionTarget = GetTargetedFunctionByNameWithinShaderAndItsFamily(clonedShader, functionToReplace->GetName());
                    if (functionTarget == nullptr) {
                        error(string("OpFunctionCallThroughCompositionVariable: cannot retrieve the function in the cloned shader. Function name: ") + functionToReplace->GetName());
                        return true;
                    }

                    //If the function is overriden by another, change the target
                    if (functionTarget->GetOverridingFunction() != nullptr) functionTarget = functionTarget->GetOverridingFunction();

                    int wordCount = asWordCount(start);
                    vecStripRanges.push_back(range_t(start + 4, start + 6 + 1));   //will remove composition variable ids from the bytecode
                    setOpAndWordCount(start, spv::OpFunctionCall, wordCount - 3);  //change instruction OpFunctionCallThroughCompositionVariable to OpFunctionCall
                    setId(start + 3, functionTarget->GetId());                     //replace the function called id

                    break;
                }
            }
            start += wordCount;
        }

        if (vecStripRanges.size() > 0)
        {
            //remove the stripped bytecode first (to have a consistent bytecode)
            stripBytecode(vecStripRanges);

            if (errorMessages.size() > 0) {
                return error("ApplyAllCompositions: failed to retarget the functions called by the compositions");
            }

            //Update all maps (update objects position in the bytecode)
            if (!UpdateAllMaps()) return error("ApplyAllCompositions: failed to update all maps");
        }
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

//Mixin is finalized: no more updates will be brought to the mixin bytecode after
bool SpxStreamRemapper::CompileMixinForStages(vector<XkslMixerOutputStage>& outputStages)
{
    //if (status != SpxRemapperStatusEnum::MixinBeingCompiled_UnusedShaderRemoved) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinFinalized;

    if (outputStages.size() == 0) return error("no output stages defined");
    if (!ValidateHeader()) return error("Failed to validate the header");

    //===================================================================================================================
    //===================================================================================================================
    //Convert SPIRX extensions to SPIRV: remove SPIRX extended instructions
    {
        vector<range_t> vecStripRanges;
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpFunctionCallBaseResolved:
                {
                    //change OpFunctionCallBaseResolved to OpFunctionCall
                    setOpCode(start, spv::OpFunctionCall);
                    break;
                }
                case spv::OpFunctionCallThroughCompositionVariable:
                {
                    error(string("Found unresolved OpFunctionCallThroughCompositionVariable at: ") + to_string(start));
                    break;
                }
                case spv::OpFunctionCallBaseUnresolved:
                {
                    error(string("A function base call has an unresolved state at: ") + to_string(start));
                    break;
                }
                case spv::OpForEachCompositionStartLoop:
                {
                    error(string("A foreach start loop has not been processed at: ") + to_string(start));
                    break;
                }
                case spv::OpForEachCompositionEndLoop:
                {
                    error(string("A foreach end loop has not been processed at: ") + to_string(start));
                    break;
                }
                case spv::OpBelongsToShader:
                case spv::OpDeclarationName:
                case spv::OpTypeXlslShaderClass:
                case spv::OpShaderInheritance:
                case spv::OpShaderCompositionDeclaration:
                case spv::OpShaderCompositionInstance:
                case spv::OpMethodProperties:
                case spv::OpMemberProperties:
                case spv::OpMemberSemanticName:
                {
                    stripInst(vecStripRanges, start);
                    break;
                }
            }
            start += wordCount;
        }
        if (errorMessages.size() > 0) return false;

        //warning: objects bytecode position is not correct anymore after this 
        stripBytecode(vecStripRanges);
    }

    //===================================================================================================================
    //===================================================================================================================
    // Generate the SPIRV bytecode for all stages
    for (unsigned int i = 0; i<outputStages.size(); ++i)
    {
        XkslMixerOutputStage& outputStage = outputStages[i];
        FunctionInstruction* entryFunction = outputStages[i].entryFunction;
        bool success = this->GenerateSpvStageBytecode(outputStage.outputStage->stage, outputStage.outputStage->entryPointName, entryFunction, outputStage.outputStage->resultingBytecode);
        if (!success)
        {
            error(string("Fail to generate SPV stage bytecode for the stage: ") + GetShadingStageLabel(outputStage.outputStage->stage));
        }
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

//For every call to a function using base accessor (base.function()), we check if we need to redirect to another overriding method
bool SpxStreamRemapper::UpdateFunctionCallsHavingUnresolvedBaseAccessor()
{
    unsigned int start = header_size;
    const unsigned int end = spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        switch (opCode)
        {
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
        start += wordCount;
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions()
{
    vector<FunctionInstruction*> vecFunctionIdBeingOverriden;
    vecFunctionIdBeingOverriden.resize(bound(), nullptr);
    bool anyOverridingFunction = false;
    for (auto itfn = vecAllFunctions.begin(); itfn != vecAllFunctions.end(); itfn++)
    {
        FunctionInstruction* function = *itfn;
        if (function->GetOverridingFunction() != nullptr)
        {
            vecFunctionIdBeingOverriden[function->GetResultId()] = function->GetOverridingFunction();
            anyOverridingFunction = true;
        }
    }

    if (!anyOverridingFunction) return true; //nothing to override

    unsigned int start = header_size;
    const unsigned int end = spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        switch (opCode)
        {
            // call to base function (OpFunctionCallBaseResolved, OpFunctionCallBaseUnresolved) are ignored

            case spv::OpFunctionCall:
            case spv::OpFunctionCallThroughCompositionVariable:
            {
                spv::Id functionCalledId = asId(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                if (functionCalledId < 0 || functionCalledId >= vecFunctionIdBeingOverriden.size()){
                    error(string("function call Id is out of bound. Id: ") + to_string(functionCalledId));
                    break;
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
        start += wordCount;
    }

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

//Return the full dependency of the shader from 
// - the complete family tree (parents of all shaders)
// - the composition type and instances of all shaders
// - for all shaders: parse the function's bytecode and detect if they're calling any other shaders
bool SpxStreamRemapper::GetShadersFullDependencies(SpxStreamRemapper* bytecodeSource, const vector<ShaderClassData*> listShaders, vector<ShaderClassData*>& fullDependencies)
{
    fullDependencies.clear();

    //setup all shaders
    for (auto itsh = bytecodeSource->vecAllShaders.begin(); itsh != bytecodeSource->vecAllShaders.end(); itsh++){
        ShaderClassData* aShader = *itsh;
        aShader->flag = 0;
        aShader->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Undefined;
    }

    //setup list of initial shader
    vector<ShaderClassData*> vecCompositionShaderInstances;
    vector<ShaderClassData*> listShaderToValidate;
    for (auto itsh = listShaders.begin(); itsh != listShaders.end(); itsh++) {
        ShaderClassData* aShaderToValidate = *itsh;
        aShaderToValidate->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
        listShaderToValidate.push_back(aShaderToValidate);
    }

    while (listShaderToValidate.size() > 0)
    {
        ShaderClassData* aShader = listShaderToValidate.back();
        listShaderToValidate.pop_back();
        if (aShader->flag != 0) continue;  //the shader has already been treated
        aShader->flag = 1;

#ifdef XKSLANG_DEBUG_MODE
        if (aShader->bytecodeSource != bytecodeSource){  //double check that the bytecode comes from the same source
            return bytecodeSource->error(string("Shader: ") + aShader->GetName() + string(" does not belong to the bytecode source"));
        }
        if (aShader->dependencyType == ShaderClassData::ShaderDependencyTypeEnum::Undefined) {
            return bytecodeSource->error(string("unset dependencyType for Shader: ") + aShader->GetName());
        }
#endif

        fullDependencies.push_back(aShader);
        
        //add all parents
        for (auto itsh = aShader->parentsList.begin(); itsh != aShader->parentsList.end(); itsh++)
        {
            ShaderClassData* aShaderParent = *itsh;
            aShaderParent->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
            if (aShaderParent->flag == 0) listShaderToValidate.push_back(aShaderParent);
        }

        //add all compositions type
        for (auto itc = aShader->compositionsList.begin(); itc != aShader->compositionsList.end(); itc++)
        {
            const ShaderComposition& aComposition = *itc;
            aComposition.shaderType->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
            if (aComposition.shaderType->flag == 0) listShaderToValidate.push_back(aComposition.shaderType);

            //add the composition instances
            if (aComposition.countInstances > 0)
            {
                if (!bytecodeSource->GetAllShaderInstancesForComposition(&aComposition, vecCompositionShaderInstances)) {
                    return bytecodeSource->error(string("Failed to retrieve the instances for the composition: ") + aComposition.variableName +
                        string(" from shader: ") + aComposition.compositionShaderOwner->GetName());
                }

                for (unsigned int i = 0; i < vecCompositionShaderInstances.size(); ++i)
                {
                    ShaderClassData* aShaderCompositionInstance = vecCompositionShaderInstances[i];
                    aShaderCompositionInstance->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
                    if (aShaderCompositionInstance->flag == 0) listShaderToValidate.push_back(aShaderCompositionInstance);
                }
            }
        }

        //if a function has been overrided by a subclass, include the subclass as well
        for (auto itf = aShader->functionsList.begin(); itf != aShader->functionsList.end(); itf++)
        {
            FunctionInstruction* aFunctionFromShader = *itf;
            FunctionInstruction* overridingFunction = aFunctionFromShader->GetOverridingFunction();
            if (overridingFunction != nullptr){
                overridingFunction->shaderOwner->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
                if (overridingFunction->shaderOwner->flag == 0) listShaderToValidate.push_back(overridingFunction->shaderOwner);
            }
        }

        //check the functions's bytecode, looking for function calls to static class
        for (auto itf = aShader->functionsList.begin(); itf != aShader->functionsList.end(); itf++)
        {
            FunctionInstruction* aFunctionFromShader = *itf;
            
            unsigned int start = aFunctionFromShader->bytecodeStartPosition;
            const unsigned int end = aFunctionFromShader->bytecodeEndPosition;
            while (start < end)
            {
                unsigned int wordCount = bytecodeSource->asWordCount(start);
                spv::Op opCode = bytecodeSource->asOpCode(start);

                switch (opCode)
                {
                    case spv::OpFunctionCall:
                    case spv::OpFunctionCallBaseResolved:
                    case spv::OpFunctionCallBaseUnresolved:
                    case spv::OpFunctionCallThroughCompositionVariable:
                    {
                        spv::Id functionCalledId = bytecodeSource->asId(start + 3);
                        FunctionInstruction* functionCalled = bytecodeSource->GetFunctionById(functionCalledId);
                        if (functionCalled == nullptr) {
                            return bytecodeSource->error(string("Failed to retrieve the function for Id: ") + to_string(functionCalledId));
                        }

                        ShaderClassData* functionShaderOwner = functionCalled->shaderOwner;
                        if (functionShaderOwner != nullptr && functionShaderOwner->flag == 0)
                        {
                            //if we're calling a static function, set the dependency type to static (unless it was already set to another dependency type)
                            if (functionCalled->IsStatic() && functionShaderOwner->dependencyType == ShaderClassData::ShaderDependencyTypeEnum::Undefined)
                                functionShaderOwner->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::StaticFunctionCall;
                            else
                                functionShaderOwner->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
                            listShaderToValidate.push_back(functionShaderOwner);
                        }

                        break;
                    }
                }
                start += wordCount;
            }
        }
    }

    return true;
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

bool SpxStreamRemapper::GetFunctionLabelAndReturnInstructionsPosition(FunctionInstruction* function, unsigned int& labelPos, unsigned int& returnPos)
{
    int state = 0;
    unsigned int start = function->bytecodeStartPosition;
    const unsigned int end = function->bytecodeEndPosition;
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);
        switch (opCode)
        {
            case spv::OpLabel:
            {
                if (state != 0) return error("Failed to parse the function: expecting OpLabel instruction");
                labelPos = start;
                state = 1;
                break;
            }
            case spv::OpReturn:
            case spv::OpReturnValue:
            {
                if (state != 1) return error("Failed to parse the function: expecting OpReturn or OpReturnValue instruction");
                returnPos = start;
                state = 2;
                break;
            }
        }

        start += wordCount;
    }

    if (state != 2) return error("Failed to parse the function: missing some opInstructions");
    return true;
}

SpxStreamRemapper::FunctionInstruction* SpxStreamRemapper::GetShaderFunctionForEntryPoint(string entryPointName)
{
    // Search for the entry point function (assume the first function with the name is the one)
    FunctionInstruction* entryPointFunction = nullptr;
    for (auto it = vecAllFunctions.begin(); it != vecAllFunctions.end(); it++)
    {
        FunctionInstruction* func = *it;
        if (func->shaderOwner == nullptr) continue;  //we're looking for a function owned by a shader

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
    //save the current bytecode
    vector<uint32_t> bytecodeBackup;// = output.getWritableBytecodeStream();
    bytecodeBackup.insert(bytecodeBackup.end(), spv.begin(), spv.end());

    //==========================================================================================
    // Update the shader stage header
    if (!BuildAndSetShaderStageHeader(stage, entryPointFunction, entryPointName))
    {
        error("Error buiding the shader stage header");
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    //==========================================================================================
    //Clean the bytecode
    if (!CleanBytecodeFromAllUnusedStuff())
    {
        error("Error cleaning the bytecode");
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    //copy the spv bytecode into the output
    vector<uint32_t>& outputSpv = output.getWritableBytecodeStream();
    outputSpv.clear();
    outputSpv.insert(outputSpv.end(), spv.begin(), spv.end());

    //reset the initial spx bytecode
    spv.clear();
    spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());

    return true;
}

bool SpxStreamRemapper::CleanBytecodeFromAllUnusedStuff()
{
    //buildLocalMaps();
    //if (staticErrorMessages.size() > 0) {
    //    copyStaticErrorMessagesTo(errorMessages);
    //    spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
    //    return false;
    //}

    //initially spirvbin_t.buildLocalMaps() function
    //build the local map, using spirvbin_t maps
    //TODO: maybe get free from spirvbin_t (we're almost not using it anymore)
    mapped.clear();
    idMapL.clear();
    fnPos.clear();
    fnCalls.clear();
    typeConstPos.clear();
    idPosR.clear();
    entryPoint = spv::NoResult;
    largestNewId = 0;

    idMapL.resize(bound(), unused);

    {
        int         fnStart = 0;
        spv::Id     fnRes = spv::NoResult;

        spv::Op opCode;
        unsigned int wordCount;
        vector<spv::Id> listIds;
        spv::Id typeId, resultId;
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            listIds.clear();
            if (!parseInstruction(start, opCode, wordCount, typeId, resultId, listIds)) return error("Error parsing the instruction");

            if (resultId != spv::spirvbin_t::unused)
            {
                idPosR[resultId] = start;
                if (typeId != spv::spirvbin_t::unused) {
                    const unsigned idTypeSize = typeSizeInWords(typeId);
                    if (idTypeSize != 0)
                        idTypeSizeMap[resultId] = idTypeSize;
                }
            }

#ifdef XKSLANG_DEBUG_MODE
            if (opCode == spv::Op::OpFunctionCallBaseUnresolved || opCode == spv::Op::OpFunctionCallBaseResolved || opCode == spv::Op::OpFunctionCallThroughCompositionVariable) {
                return error("Invalid functionCall opCode");
            }
#endif

            if (opCode == spv::Op::OpFunctionCall) {
                ++fnCalls[asId(start + 3)];
            }
            else if (opCode == spv::Op::OpEntryPoint) {
                entryPoint = asId(start + 2);
            }
            else if (opCode == spv::Op::OpFunction) {
                if (fnStart != 0) return error("nested function found");
                fnStart = start;
                fnRes = asId(start + 2);
            }
            else if (opCode == spv::Op::OpFunctionEnd) {
#ifdef XKSLANG_DEBUG_MODE
                if (fnRes == spv::NoResult) return error("invalid fnRes");
                if (fnStart == 0) return error("function end without function start");
#endif
                fnPos[fnRes] = range_t(fnStart, start + asWordCount(start));
                fnStart = 0;
                fnRes = spv::NoResult;
            }
            else if (isConstOp(opCode)) {
                typeConstPos.insert(start);
            }
            else if (isTypeOp(opCode)) {
                typeConstPos.insert(start);
            }

            if (typeId != spv::spirvbin_t::unused) localId(typeId, unmapped);
            if (resultId != spv::spirvbin_t::unused) localId(resultId, unmapped);
            for (unsigned int i = 0; i < listIds.size(); ++i) localId(listIds[i], unmapped);

            start += wordCount;
        }
    }

    dceFuncs(); //dce uncalled functions
    dceVars();  //dce unused function variables + decorations / name
    dceTypes(); //dce unused types

    strip();         //remove all strip bytecode
    stripDeadRefs(); //remove references to things we DCEed

    //change the range of all remaining IDs
    mapRemainder(); // map any unmapped IDs
    applyMap();     // Now remap each shader to the new IDs we've come up with

    if (errorMessages.size() > 0) return false;
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

    //remove the current entry points (if any)
    {
        vector<range_t> vecStripRanges;
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpEntryPoint:
                {
                    stripInst(vecStripRanges, start);
                    break;
                }

                case spv::OpTypeXlslShaderClass:
                case spv::OpDeclarationName:
                {
                    //all entry points are set before those declarations: we can stop parsing the rest of the bytecode
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }

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

    //We build the hashmap table for all types and consts
    //except for OpTypeXlslShaderClass types: (this type is only informational, never used as a type or result)
    unsigned int start = header_size;
    const unsigned int end = spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        spv::Id id = spv::spirvbin_t::unused;
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

        if (id != spv::spirvbin_t::unused)
        {
            const uint32_t hashval = hashType(start);
#ifdef XKSLANG_DEBUG_MODE
            if (mapHashPos.find(hashval) != mapHashPos.end())
            {
                // Warning: might cause some conflicts sometimes?
                //return error(string("2 types have the same hashmap value. Ids: ") + to_string(mapHashPos[hashval].first) + string(", ") + to_string(id));
                id = spv::spirvbin_t::unused;  //by precaution we invalidate the id: we should not choose between them
            }
#endif
            mapHashPos[hashval] = pairIdPos(id, start);
        }

        start += wordCount;
    }

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
    unsigned int countParsedObjects = listParsedObjectsData.size();
    for (unsigned int i = 0; i < countParsedObjects; ++i)
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

    unsigned int countParsedObjects = listParsedObjectsData.size();
    for (unsigned int i = 0; i < countParsedObjects; ++i)
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

    unsigned int countParsedObjects = listParsedObjectsData.size();
    for (unsigned int i = 0; i < countParsedObjects; ++i)
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

    unsigned int start = header_size;
    const unsigned int end = spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        switch (opCode)
        {
            /*case spv::OpMemberProperties:
            {
                //if the member has a stream property, define the type as a stream struct
                const spv::Id typeId = asId(start + 1);
                if (typeId < 0 || typeId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[typeId]) break;

                break;
            }*/

            case spv::Op::OpBelongsToShader:
            {
                const spv::Id shaderId = asId(start + 1);
                const spv::Id objectId = asId(start + 2);

                if (shaderId < 0 || shaderId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[shaderId]) break;

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
                        if (typePointer->GetShaderOwner() != nullptr) { error(string("The typePointer already has a shader owner: ") + typePointer->GetName()); break; }
                        if (variable->GetShaderOwner() != nullptr) { error(string("The variable already has a shader owner: ") + variable->GetName()); break; }
                        if (shaderOwner->HasType(type)) { error(string("The shader: ") + shaderOwner->GetName() + string(" already possesses the type: ") + type->GetName()); break; }
#endif
                        ShaderTypeData* shaderType = new ShaderTypeData(type, typePointer, variable);
                        type->SetShaderOwner(shaderOwner);
                        typePointer->SetShaderOwner(shaderOwner);
                        variable->SetShaderOwner(shaderOwner);
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

                if (shaderId < 0 || shaderId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[shaderId]) break;

                ShaderClassData* shader = GetShaderById(shaderId);
                if (shader == nullptr) { error(string("undeclared shader for Id: ") + to_string(shaderId)); break; }

                int countParents = wordCount - 2;
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

            case spv::OpShaderCompositionDeclaration:
            {
                const spv::Id shaderId = asId(start + 1);

                if (shaderId < 0 || shaderId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[shaderId]) break;

                ShaderClassData* shaderCompositionOwner = GetShaderById(shaderId);
                if (shaderCompositionOwner == nullptr) { error(string("undeclared shader id: ") + to_string(shaderId)); break; }

                int compositionId = asLiteralValue(start + 2);

                const spv::Id shaderCompositionTypeId = asId(start + 3);
                ShaderClassData* shaderCompositionType = GetShaderById(shaderCompositionTypeId);
                if (shaderCompositionType == nullptr) { error(string("undeclared shader type id: ") + to_string(shaderCompositionTypeId)); break; }

                bool isArray = asLiteralValue(start + 4) == 0? false: true;

                int count = asLiteralValue(start + 5);

                const string compositionVariableName = literalString(start + 6);

                ShaderComposition shaderComposition(compositionId, shaderCompositionOwner, shaderCompositionType, compositionVariableName, isArray, count);
                shaderCompositionOwner->AddComposition(shaderComposition);
                break;
            }

            //case spv::OpMemberSemanticName:
            //case spv::OpMemberProperties:
            //{
            //    return error(string("unprocessed yet"));
            //}

            case spv::Op::OpMethodProperties:
            {
                //a function is defined with some properties
                const spv::Id functionId = asId(start + 1);

                if (functionId < 0 || functionId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[functionId]) break;

                FunctionInstruction* function = GetFunctionById(functionId);
                if (function == nullptr) { error(string("undeclared function id: ") + to_string(functionId)); break; }

                int countProperties = wordCount - 2;
                for (int a = 0; a < countProperties; ++a)
                {
                    int prop = asLiteralValue(start + 2 + a);
                    switch (prop)
                    {
                    case spv::XkslPropertyEnum::PropertyMethodOverride:
                        function->ParsedOverrideAttribute();
                        break;
                    case spv::XkslPropertyEnum::PropertyMethodStatic:
                        function->ParsedStaticAttribute();
                        break;
                    }
                }
                break;
            }
        }

        start += wordCount;
    }

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
            
            bool isS32 = false; int valueS32 = 0;
            //if the const if a signed 32 bits, prefetch its numeric value
            {
                spv::Id constTypeId = asId(parsedData.bytecodeStartPosition + 1);
                TypeInstruction* constType = GetTypeById(constTypeId);
                if (constType == nullptr) {
                    error(string("no type exist for const typeId: ") + to_string(constTypeId));
                    return nullptr;
                }
                spv::Op typeOpCode = asOpCode(constType->bytecodeStartPosition);
                if (typeOpCode == spv::OpTypeInt)
                {
                    int literalNumber = asLiteralValue(constType->bytecodeStartPosition + 2);
                    int sign = asLiteralValue(constType->bytecodeStartPosition + 3);
                    if (literalNumber == 32 && sign == 1) {
                        isS32 = true;
                        valueS32 = asLiteralValue(parsedData.bytecodeStartPosition + 3);
                    }
                }
            }

            newObject = new ConstInstruction(parsedData, declarationName, this, isS32, valueS32);
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
            if (!hasDeclarationName || declarationName.size() == 0)
                declarationName = "__globalFunction__" + to_string(vecAllFunctions.size());

            FunctionInstruction* function = new FunctionInstruction(parsedData, declarationName, this);
            vecAllFunctions.push_back(function);

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

    unsigned int start = header_size;
    const unsigned int end = spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        unsigned end = start + wordCount;

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

        start += wordCount;
    }

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

SpxStreamRemapper::FunctionInstruction* SpxStreamRemapper::GetTargetedFunctionByNameWithinShaderAndItsFamily(ShaderClassData* shader, const string& name)
{
    FunctionInstruction* function = shader->GetFunctionByName(name);
    if (function != nullptr) {
        if (function->GetOverridingFunction() != nullptr) return function->GetOverridingFunction();
        return function;
    }

    //Look for the function within the shader parents
    for (unsigned int i = 0; i < shader->parentsList.size(); ++i)
    {
        function = GetTargetedFunctionByNameWithinShaderAndItsFamily(shader->parentsList[i], name);
        if (function != nullptr) return function;
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

bool SpxStreamRemapper::GetAllCompositionForEachLoops(vector<CompositionForEachLoopData>& vecForEachLoops, int& maxForEachLoopsNestedLevel)
{
    vecForEachLoops.clear();
    maxForEachLoopsNestedLevel = -1;

    int currentForEachLoopNestedLevel = -1;
    vector<CompositionForEachLoopData> pileForeachLoopsCurrentlyParsed;
    unsigned int start = header_size;
    const unsigned int end = spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        switch (opCode)
        {
            case spv::OpForEachCompositionStartLoop:
            {
                spv::Id shaderId = asId(start + 1);
                int compositionId = asLiteralValue(start + 2);

                ShaderComposition* composition = GetCompositionById(shaderId, compositionId);
                if (composition == nullptr) {
                    error(string("Composition not found for ShaderId: ") + to_string(shaderId) + string(" with composition id: ") + to_string(compositionId));
                    break;
                }
                if (!composition->isArray) { error("Foreach loop only works with array compositions."); break; }

                currentForEachLoopNestedLevel++;
                if (currentForEachLoopNestedLevel > maxForEachLoopsNestedLevel) maxForEachLoopsNestedLevel = currentForEachLoopNestedLevel;
                pileForeachLoopsCurrentlyParsed.push_back(CompositionForEachLoopData(composition, currentForEachLoopNestedLevel, start, 0, start + wordCount, 0));

                break;
            }

            case spv::OpForEachCompositionEndLoop:
            {
                if (currentForEachLoopNestedLevel < 0) { error("A foreach end loop instruction is missing a start instruction"); break; }
                currentForEachLoopNestedLevel--;

                CompositionForEachLoopData compositionForEachLoop = pileForeachLoopsCurrentlyParsed.back();
                pileForeachLoopsCurrentlyParsed.pop_back();

                compositionForEachLoop.lastLoopInstuctionEnd = start;
                compositionForEachLoop.foreachLoopEnd = start + wordCount;
                vecForEachLoops.push_back(compositionForEachLoop);

                break;
            }
        }

        start += wordCount;
    }

    if (currentForEachLoopNestedLevel >= 0) error("A foreach start loop instruction is missing an end instruction");
    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::GetAllShaderInstancesForComposition(const ShaderComposition* composition, vector<ShaderClassData*>& instances)
{
    spv::Id compositionShaderOwnerId = composition->compositionShaderOwner->GetId();
    int compositionShaderId = composition->compositionShaderId;

    int countInstancesExpected = composition->countInstances;
    instances.resize(countInstancesExpected, nullptr);
    int countInstancesFound = 0;

    //look for the instances in the bytecode
    unsigned int start = header_size;
    const unsigned int end = spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        switch (opCode)
        {
            case spv::OpShaderCompositionInstance:
            {
                const spv::Id shaderOwnerId = asId(start + 1);
                const int compositionId = asLiteralValue(start + 2);

                if (shaderOwnerId == compositionShaderOwnerId && compositionId == compositionShaderId)
                {
                    int instanceNum = asLiteralValue(start + 3);
                    if (instanceNum < 0 || instanceNum >= countInstancesExpected)
                        return error(string("composition instance has an invalid num: ") + to_string(instanceNum));
                    if (instances[instanceNum] != nullptr)
                        return error(string("Found 2 instances for the same composition having the same num: ") + to_string(instanceNum));

                    const spv::Id shaderInstanceId = asId(start + 4);
                    ShaderClassData* shaderInstance = GetShaderById(shaderInstanceId);
                    if (shaderInstance == nullptr)
                        return error(string("unfound composition shader instance for id: ") + to_string(shaderInstanceId));
                    instances[instanceNum] = shaderInstance;
                    countInstancesFound++;
                }

                break;
            }

            case spv::OpTypeXlslShaderClass:
            {
                //all declaration must be set before type declaration: we can stop parsing the rest of the bytecode
                start = end;
                break;
            }
        }
        start += wordCount;
    }

    if (countInstancesExpected != countInstancesFound) {
        return error(string("Invalid number of instances found. Expected number: ") + to_string(countInstancesExpected) + string(". Instances found: ") + to_string(countInstancesFound));
    }

    return true;
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

SpxStreamRemapper::ConstInstruction* SpxStreamRemapper::GetConstById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Const)
    {
        ConstInstruction* constInstr = dynamic_cast<ConstInstruction*>(obj);
        return constInstr;
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

bool SpxStreamRemapper::remapAllIds(vector<uint32_t>& bytecode, unsigned begin, unsigned end, const vector<spv::Id>& remapTable)
{
    string errorMsg;
    if (!remapAllIds(bytecode, begin, end, remapTable, errorMsg))
    {
        error(errorMsg);
        return false;
    }
    return true;
}

bool SpxStreamRemapper::remapAllIds(vector<uint32_t>& bytecode, unsigned begin, unsigned end, const vector<spv::Id>& remapTable, string& errorMsg)
{
    unsigned int pos = begin;
    unsigned int wordCount = 0;
    while (pos < end)
    {
        if (!remapAllInstructionIds(bytecode, pos, wordCount, remapTable, errorMsg)) return false;
        pos += wordCount;
    }

    return true;
}

// inspired by spirvbin_t::processInstruction. Allow us to remap the instruction Ids
bool SpxStreamRemapper::remapAllInstructionIds(vector<uint32_t>& bytecode, unsigned word, unsigned& wordCount, const vector<spv::Id>& remapTable, string& errorMsg)
{
    const auto instructionStart = word;
    wordCount = opWordCount(bytecode[instructionStart]);
    spv::Op opCode = opOpCode(bytecode[instructionStart]);
    const unsigned int nextInst = word++ + wordCount;

    if (nextInst > bytecode.size())
    {
        errorMsg = "nextInst is out of bound";
        return false;
    }

    // Base for computing number of operands; will be updated as more is learned
    unsigned numOperands = wordCount - 1;

    // Read type and result ID from instruction desc table
    if (spv::InstructionDesc[opCode].hasType()) {
        //listIds.push_back(asId(word++));
        //type = asId(word++);
        bytecode[word] = remapTable[bytecode[word]];
        word++;
        --numOperands;
    }

    if (spv::InstructionDesc[opCode].hasResult()) {
        //listIds.push_back(asId(word++));
        //result = asId(word++);
        bytecode[word] = remapTable[bytecode[word]];
        word++;
        --numOperands;
    }

    // Extended instructions: currently, assume everything is an ID.
    // TODO: add whatever data we need for exceptions to that
    if (opCode == spv::OpExtInst) {
        //listIds.push_back(asId(word));  //Add ExtInstImport Id
        bytecode[word] = remapTable[bytecode[word]];

        word += 2; // instruction set, and instruction from set
        numOperands -= 2;

        for (unsigned op = 0; op < numOperands; ++op)
        {
            bytecode[word] = remapTable[bytecode[word]]; word++;
        }

        return true;
    }

    // Circular buffer so we can look back at previous unmapped values during the mapping pass.
    static const unsigned idBufferSize = 4;
    spv::Id idBuffer[idBufferSize];
    unsigned idBufferPos = 0;

    // Store IDs from instruction in our map
    for (int op = 0; numOperands > 0; ++op, --numOperands) {
        switch (spv::InstructionDesc[opCode].operands.getClass(op)) {
        case spv::OperandId:
        case spv::OperandScope:
        case spv::OperandMemorySemantics:
            idBuffer[idBufferPos] = bytecode[word];
            idBufferPos = (idBufferPos + 1) % idBufferSize;
            bytecode[word] = remapTable[bytecode[word]];
            word++;
            break;

        case spv::OperandVariableIds:
            for (unsigned i = 0; i < numOperands; ++i)
            {
                bytecode[word] = remapTable[bytecode[word]]; word++;
            }
            return true;

        case spv::OperandVariableLiterals:
            // for clarity
            // if (opCode == spv::OpDecorate && asDecoration(word - 1) == spv::DecorationBuiltIn) {
            //     ++word;
            //     --numOperands;
            // }
            // word += numOperands;
            return true;

        case spv::OperandVariableLiteralId: {
            if (opCode == spv::OpSwitch)
            {
                /// // word-2 is the position of the selector ID.  OpSwitch Literals match its type.
                /// // In case the IDs are currently being remapped, we get the word[-2] ID from
                /// // the circular idBuffer.
                /// const unsigned literalSizePos = (idBufferPos + idBufferSize - 2) % idBufferSize;
                /// const unsigned literalSize = idTypeSizeInWords(idBuffer[literalSizePos]);
                /// const unsigned numLiteralIdPairs = (nextInst - word) / (1 + literalSize);
                /// 
                /// for (unsigned arg = 0; arg<numLiteralIdPairs; ++arg) {
                ///     word += literalSize;  // literal
                ///     spv[word] = remapTable[spv[word]];   // label
                ///     word++;
                /// }
                errorMsg = "Unplugged operand for now.. to check later";
                return false;
            }
            else {
                errorMsg = "invalid OperandVariableLiteralId instruction";
                return false;
            }

            return true;
        }

        case spv::OperandLiteralString: {
            const int stringWordCount = literalStringWords(literalString(bytecode, word));
            word += stringWordCount;
            numOperands -= (stringWordCount - 1); // -1 because for() header post-decrements
            break;
        }

        // Execution mode might have extra literal operands.  Skip them.
        case spv::OperandExecutionMode:
            return true;

        // Single word operands we simply ignore, as they hold no IDs
        case spv::OperandLiteralNumber:
        case spv::OperandSource:
        case spv::OperandExecutionModel:
        case spv::OperandAddressing:
        case spv::OperandMemory:
        case spv::OperandStorage:
        case spv::OperandDimensionality:
        case spv::OperandSamplerAddressingMode:
        case spv::OperandSamplerFilterMode:
        case spv::OperandSamplerImageFormat:
        case spv::OperandImageChannelOrder:
        case spv::OperandImageChannelDataType:
        case spv::OperandImageOperands:
        case spv::OperandFPFastMath:
        case spv::OperandFPRoundingMode:
        case spv::OperandLinkageType:
        case spv::OperandAccessQualifier:
        case spv::OperandFuncParamAttr:
        case spv::OperandDecoration:
        case spv::OperandBuiltIn:
        case spv::OperandSelect:
        case spv::OperandLoop:
        case spv::OperandFunction:
        case spv::OperandMemoryAccess:
        case spv::OperandGroupOperation:
        case spv::OperandKernelEnqueueFlags:
        case spv::OperandKernelProfilingInfo:
        case spv::OperandCapability:
            ++word;
            break;

        case spv::OperandProperties:
            return true;

        default:
            errorMsg = "Unhandled Operand Class";
            return false;
        }
    }

    return true;
}

// inspired by spirvbin_t::processInstruction. Allow us to store the instruction data and ids
bool SpxStreamRemapper::parseInstruction(const vector<uint32_t>& bytecode, unsigned word, spv::Op& opCode, unsigned& wordCount, spv::Id& type, spv::Id& result, vector<spv::Id>& listIds, string& errorMsg)
{
    const auto instructionStart = word;
    wordCount = opWordCount(bytecode[instructionStart]);
    opCode = opOpCode(bytecode[instructionStart]);
    const unsigned int nextInst = word++ + wordCount;

    if (nextInst > bytecode.size())
    {
        errorMsg = "nextInst is out of bound";
        return false;
    }

    // Base for computing number of operands; will be updated as more is learned
    unsigned numOperands = wordCount - 1;

    // Read type and result ID from instruction desc table
    if (spv::InstructionDesc[opCode].hasType()) {
        //listIds.push_back(asId(word++));
        type = bytecode[word++];
        --numOperands;
    }
    else type = unused;

    if (spv::InstructionDesc[opCode].hasResult()) {
        //listIds.push_back(asId(word++));
        result = bytecode[word++];
        --numOperands;
    }
    else result = unused;

    // Extended instructions: currently, assume everything is an ID.
    // TODO: add whatever data we need for exceptions to that
    if (opCode == spv::OpExtInst) {
        listIds.push_back(bytecode[word]);  //Add ExtInstImport Id

        word += 2; // instruction set, and instruction from set
        numOperands -= 2;

        for (unsigned op = 0; op < numOperands; ++op)
            listIds.push_back(bytecode[word++]); // ID

        return true;
    }

    // Circular buffer so we can look back at previous unmapped values during the mapping pass.
    static const unsigned idBufferSize = 4;
    spv::Id idBuffer[idBufferSize];
    unsigned idBufferPos = 0;

    // Store IDs from instruction in our map
    for (int op = 0; numOperands > 0; ++op, --numOperands)
    {
        const spv::OperandParameters& operands = spv::InstructionDesc[opCode].operands;
#ifdef XKSLANG_DEBUG_MODE
        if (op >= operands.getNum()) {
            errorMsg = "op is out of bound";
            return false;
        }
#endif
        switch (operands.getClass(op))
        {
            case spv::OperandId:
            case spv::OperandScope:
            case spv::OperandMemorySemantics:
                idBuffer[idBufferPos] = bytecode[word];
                idBufferPos = (idBufferPos + 1) % idBufferSize;
                listIds.push_back(bytecode[word++]);
                break;

            case spv::OperandVariableIds:
                for (unsigned i = 0; i < numOperands; ++i)
                    listIds.push_back(bytecode[word++]);
                return true;

            case spv::OperandVariableLiterals:
                // for clarity
                // if (opCode == spv::OpDecorate && asDecoration(word - 1) == spv::DecorationBuiltIn) {
                //     ++word;
                //     --numOperands;
                // }
                // word += numOperands;
                return true;

            case spv::OperandVariableLiteralId: {
                if (opCode == spv::OpSwitch)
                {
                    /// // word-2 is the position of the selector ID.  OpSwitch Literals match its type.
                    /// // In case the IDs are currently being remapped, we get the word[-2] ID from
                    /// // the circular idBuffer.
                    /// const unsigned literalSizePos = (idBufferPos + idBufferSize - 2) % idBufferSize;
                    /// const unsigned literalSize = idTypeSizeInWords(idBuffer[literalSizePos]);
                    /// const unsigned numLiteralIdPairs = (nextInst - word) / (1 + literalSize);
                    /// 
                    /// for (unsigned arg = 0; arg<numLiteralIdPairs; ++arg) {
                    ///     word += literalSize;  // literal
                    ///     listIds.push_back(bytecode[word++]);   // label
                    /// }
                    errorMsg = "Unplugged operand for now.. to check later";
                    return false;
                }
                else {
                    errorMsg = "invalid OperandVariableLiteralId instruction";
                    return false;
                }

                return true;
            }

            case spv::OperandLiteralString: {
                const int stringWordCount = literalStringWords(literalString(bytecode, word));
                word += stringWordCount;
                numOperands -= (stringWordCount - 1); // -1 because for() header post-decrements
                break;
            }

            // Execution mode might have extra literal operands.  Skip them.
            case spv::OperandExecutionMode:
                return true;

            // Single word operands we simply ignore, as they hold no IDs
            case spv::OperandLiteralNumber:
            case spv::OperandSource:
            case spv::OperandExecutionModel:
            case spv::OperandAddressing:
            case spv::OperandMemory:
            case spv::OperandStorage:
            case spv::OperandDimensionality:
            case spv::OperandSamplerAddressingMode:
            case spv::OperandSamplerFilterMode:
            case spv::OperandSamplerImageFormat:
            case spv::OperandImageChannelOrder:
            case spv::OperandImageChannelDataType:
            case spv::OperandImageOperands:
            case spv::OperandFPFastMath:
            case spv::OperandFPRoundingMode:
            case spv::OperandLinkageType:
            case spv::OperandAccessQualifier:
            case spv::OperandFuncParamAttr:
            case spv::OperandDecoration:
            case spv::OperandBuiltIn:
            case spv::OperandSelect:
            case spv::OperandLoop:
            case spv::OperandFunction:
            case spv::OperandMemoryAccess:
            case spv::OperandGroupOperation:
            case spv::OperandKernelEnqueueFlags:
            case spv::OperandKernelProfilingInfo:
            case spv::OperandCapability:
                ++word;
                break;

            default:
                errorMsg = "Unhandled Operand Class";
                return false;
        }
    }

    return true;
}

// inspired by spirvbin_t::processInstruction. Allow us to store the instruction data and ids
bool SpxStreamRemapper::parseInstruction(unsigned word, spv::Op& opCode, unsigned& wordCount, spv::Id& type, spv::Id& result, vector<spv::Id>& listIds)
{
    string errorMsg;
    if (!parseInstruction(spv, word, opCode, wordCount, type, result, listIds, errorMsg))
    {
        error(errorMsg);
        return false;
    }
    return true;
}