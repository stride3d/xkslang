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

bool SpxCompiler::AddComposition(const string& shaderName, const string& variableName, SpxCompiler* source)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    //===================================================================================================================
    //===================================================================================================================
    //Find the shader composition target
    ShaderComposition* compositionTarget = nullptr;
    {
        ShaderClassData* shaderCompositionTarget = nullptr;
        if (shaderName.size() > 0)
        {
            //the shader is specified by the user
            shaderCompositionTarget = this->GetShaderByName(shaderName);
            if (shaderCompositionTarget == nullptr) return error(string("No shader exists in destination bytecode with the name: ") + shaderName);

            compositionTarget = GetShaderCompositionForVariableName(shaderCompositionTarget, variableName, true);
            if (compositionTarget == nullptr) return error(string("No composition exists in shader: ") + shaderName + string(", with the name: ") + variableName);
        }
        else
        {
            //unknown shader, we look for the shader declaring the variable name (if any conflict we return an error)
            for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); ++it)
            {
                ShaderClassData* aShader = *it;
                ShaderComposition* aMatchingComposition = aShader->GetShaderCompositionByName(variableName);
                if (aMatchingComposition != nullptr)
                {
                    if (compositionTarget != nullptr) return error(string("Found at least 2 composition with the name: ") + variableName);

                    shaderCompositionTarget = aShader;
                    compositionTarget = aMatchingComposition;
                }
            }
            if (compositionTarget == nullptr) return error(string("No composition found with the name: ") + variableName);
        }
    }

#ifdef XKSLANG_DEBUG_MODE
    if (compositionTarget == nullptr || compositionTarget->compositionShaderOwner == nullptr) return error("Internal error");  //should never happen
#endif

    ShaderClassData* shaderCompositionOwner = compositionTarget->compositionShaderOwner;
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
    if (!SpxCompiler::GetShadersFullDependencies(source, listShader, shadersFullDependencies)) {
        source->copyMessagesTo(errorMessages);
        return error(string("Failed to get the shaders dependencies"));
    }

    vector<ShaderToMergeData> listShadersToMerge;
    for (auto its = shadersFullDependencies.begin(); its != shadersFullDependencies.end(); its++)
    {
        ShaderClassData* aShaderToMerge = *its;
        if (aShaderToMerge->dependencyType == ShaderClassData::ShaderDependencyTypeEnum::StaticAccess)
        {
            //if calling a shader static function only: we don't make a new instance
            if (this->GetShaderByName(aShaderToMerge->GetName()) == nullptr)
            {
                listShadersToMerge.push_back(ShaderToMergeData(aShaderToMerge, false));
            }
        }
        else
        {
            //we will make a new instance of the shader in the destination bytecode
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
    //After having instantiated a shader, we set the original shader name to all cloned cbuffers having a STAGE property (so that we can merge them later on)
    for (auto its = listShadersToMerge.begin(); its != listShadersToMerge.end(); its++)
    {
        const ShaderToMergeData& shaderInstantiated = *its;
        if (shaderInstantiated.instantiateShader)
        {
            ShaderClassData* originalShader = shaderInstantiated.shader;
            ShaderClassData* instantiatedShader = originalShader->tmpClonedShader;
            if (instantiatedShader == nullptr) return error(string("Cannot retrieve the instantiated shader after having added some compositions"));

            for (auto itt = instantiatedShader->shaderTypesList.begin(); itt != instantiatedShader->shaderTypesList.end(); itt++)
            {
                ShaderTypeData* aShaderType = *itt;
                if (aShaderType->isCBufferType())
                {
                    CBufferTypeData* cbufferData = aShaderType->GetCBufferData();
                    if (cbufferData == nullptr) return error("a cbuffer type is missing cbuffer data (block decorate but no CBufferProperties?): " + aShaderType->type->GetName());

                    if (cbufferData->isStage)
                    {
                        cbufferData->shaderOwnerName = originalShader->shaderOriginalTypeName;
                    }
                }
            }
        }
    }

    //===================================================================================================================
    //===================================================================================================================
    // - update our composition data, and add the instances instruction
    {
        //Analyse the function's bytecode, to add all new functions called
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
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

                if (shaderId == shaderCompositionOwner->GetId() && compositionId == compositionTargetId)
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

        if (!compositionUpdated) return error("The target composition has not been updated");

        if (!UpdateAllMaps()) return error("Failed to update all maps");
    }

    if (errorMessages.size() > 0) return false;
    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}


bool SpxCompiler::GetListAllCompositions(vector<ShaderComposition*>& vecCompositions)
{
    vecCompositions.clear();

    for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); ++it)
    {
        ShaderClassData* aShader = *it;
        unsigned int countCompositions = aShader->GetCountShaderComposition();
        for (unsigned int k = 0; k < countCompositions; ++k)
        {
            ShaderComposition* shaderComposition = &(aShader->compositionsList[k]);
            vecCompositions.push_back(shaderComposition);
        }
    }

    return true;
}

bool SpxCompiler::GetListAllCompositionsInfo(vector<ShaderCompositionInfo>& vecCompositionsInfo)
{
    vecCompositionsInfo.clear();

    vector<ShaderComposition*> vecCompositions;
    GetListAllCompositions(vecCompositions);

    for (auto it = vecCompositions.begin(); it != vecCompositions.end(); ++it)
    {
        ShaderComposition* shaderComposition = *it;
        vecCompositionsInfo.push_back(ShaderCompositionInfo(
            shaderComposition->compositionShaderOwner->GetName(),
            shaderComposition->shaderType->GetName(),
            shaderComposition->variableName,
            shaderComposition->isArray,
            shaderComposition->countInstances
        ));
    }

    return true;
}

bool SpxCompiler::GetAllCompositionsForVariableName(ShaderClassData* shader, const string& variableName, bool lookInParentShaders, vector<ShaderComposition*>& listCompositions)
{
#ifdef XKSLANG_DEBUG_MODE
    if (shader == nullptr) return error("Shader is null");
#endif

    for (auto itc = shader->compositionsList.begin(); itc != shader->compositionsList.end(); itc++)
    {
        ShaderComposition& aComposition = *itc;
        if (aComposition.variableName == variableName)
        {
            listCompositions.push_back(&aComposition);
        }
    }

    if (lookInParentShaders)
    {
        unsigned int countParents = shader->parentsList.size();
        for (unsigned int p = 0; p < countParents; ++p)
        {
            if (!GetAllCompositionsForVariableName(shader->parentsList[p], variableName, lookInParentShaders, listCompositions)) return false;
        }
    }

    return true;
}

SpxCompiler::ShaderComposition* SpxCompiler::GetShaderCompositionForVariableName(ShaderClassData* shader, const string& variableName, bool lookInParentShaders)
{
    vector<ShaderComposition*> listCompositions;
    if (!GetAllCompositionsForVariableName(shader, variableName, true, listCompositions))
    {
        error("Fail to get all compositions for the variable: " + variableName);
        return nullptr;
    }

    unsigned int countCompositionFound = listCompositions.size();
    /*if (countCompositionFound > 1){
    error("Several compositions found for the variable: " + variableName);
    return nullptr;
    }*/
    // ==> In the case of several compositions exists with the same variable name: we now return the 1st one found

    if (countCompositionFound == 0) return nullptr;
    return listCompositions[0];
}