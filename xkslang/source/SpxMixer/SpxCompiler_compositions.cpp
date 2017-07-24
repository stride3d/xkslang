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

bool SpxCompiler::AddCompositionInstance(const string& shaderName, const string& variableName, SpxCompiler* source)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    //===================================================================================================================
    //===================================================================================================================
    //Find the shader composition target
    ShaderCompositionDeclaration* compositionTarget = nullptr;
    {
        ShaderClassData* shaderCompositionTarget = nullptr;
        if (shaderName.size() > 0)
        {
            //the shader is specified by the user
            shaderCompositionTarget = this->GetShaderByName(shaderName);
            if (shaderCompositionTarget == nullptr) return error(string("No shader exists in destination bytecode with the name: ") + shaderName);

            compositionTarget = GetShaderCompositionDeclarationForVariableName(shaderCompositionTarget, variableName, true);
            if (compositionTarget == nullptr) return error(string("No composition exists in shader: ") + shaderName + string(", with the name: ") + variableName);
        }
        else
        {
            //unknown shader, we look for the shader declaring the variable name (if any conflict we return an error)
            for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); ++it)
            {
                ShaderClassData* aShader = *it;
                ShaderCompositionDeclaration* aMatchingComposition = aShader->GetShaderCompositionByName(variableName);
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

    //is the target composition overriden by another composition?
    if (compositionTarget->overridenBy != nullptr)
    {
        compositionTarget = compositionTarget->overridenBy;
    }

    if (compositionTarget->isArray == false && compositionTarget->countInstances > 0)
    {
#ifdef MIXIN_ADD_COMPOSITION_RETURN_AN_ERROR_IF_A_NON_ARRAYED_COMPOSITION_RECEIVES_MORE_THAN_1_INSTANCE
        return error(string("The composition has already been instanciated: " + compositionTarget->GetShaderOwnerAndVariableName()));
#else
        return true;
#endif  
    }

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
    string namePrefix = string("o") + to_string(prefixId) + string("S") + to_string(compositionTarget->compositionShaderOwner->GetId()) + string("C") + to_string(compositionTarget->compositionShaderId) + string("_");

    //Merge (duplicate) all shaders targeted by by the composition into the current bytecode
    if (!MergeShadersIntoBytecode(*source, listShadersToMerge, namePrefix))
    {
        error(string("failed to clone the shader for the composition: ") + compositionTarget->compositionShaderOwner->GetName() + string(".") + compositionTarget->variableName);
        return false;
    }

    //retrieve the merged shaders from the destination bytecode
    vector<ShaderClassData*> listShadersMerged;
    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is].shader;
        ShaderClassData* shaderMerged = shaderToMerge->tmpClonedShader;
        if (shaderMerged == nullptr) return error(string("Cannot retrieve the merged shader: ") + shaderToMerge->GetName());
        listShadersMerged.push_back(shaderMerged);
    }
    ShaderClassData* mainShaderTypeMerged = mainShaderTypeMergedToMergeFromSource->tmpClonedShader;
    if (mainShaderTypeMerged == nullptr)
        return error(string("Merge shader function is expected to return a reference on the merged shader"));

    //===================================================================================================================
    //===================================================================================================================
    // Record the new instance and update the instanced shaders
    {
        //update bytecode position of all shader composition data
        if (!UpdateCompositionDataFromBytecodeForCompositionAndShaders(compositionTarget)) return error("Failed to update the composition and shaders data bytecode position");
        unsigned int compositionBytecodePosition = compositionTarget->tmpBytecodePosition;

        BytecodeUpdateController bytecodeUpdateController;
        BytecodeChunk* bytecodeNewInstance = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, compositionBytecodePosition, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);

        spv::Id compositionShaderOwnerId = compositionTarget->compositionShaderOwner->GetId();
        int shaderCompositionId = compositionTarget->compositionShaderId;
        int compositionInstanceNum = compositionTarget->countInstances;
        compositionTarget->countInstances++;

        //add the instances OP instruction in the bytecode
        {
            // update number of instances
            SetNewAtomicValueUpdate(bytecodeUpdateController, compositionBytecodePosition + 6, compositionTarget->countInstances);

            // Add the instance information in the bytecode
            spv::Id mergedShaderTypeId = mainShaderTypeMerged->GetId();
            spv::Instruction compInstanceInstr(spv::OpShaderCompositionInstance);
            compInstanceInstr.addIdOperand(compositionShaderOwnerId);
            compInstanceInstr.addImmediateOperand(shaderCompositionId);
            compInstanceInstr.addImmediateOperand(compositionInstanceNum);
            compInstanceInstr.addIdOperand(mergedShaderTypeId);

            compInstanceInstr.dump(bytecodeNewInstance->bytecode);
        }

        //For all instantiated shaders: we add the instance information
        for (auto its = listShadersMerged.begin(); its != listShadersMerged.end(); its++)
        {
            ShaderClassData* anInstanciatedShader = *its;
            int instanceLevel = anInstanciatedShader->listInstancingPathItems.size();

            spv::Instruction shaderInstancingInstr(spv::OpShaderInstancingPathItem);
            shaderInstancingInstr.addIdOperand(anInstanciatedShader->GetId());
            shaderInstancingInstr.addImmediateOperand(instanceLevel);
            shaderInstancingInstr.addIdOperand(compositionShaderOwnerId);
            shaderInstancingInstr.addImmediateOperand(shaderCompositionId);
            shaderInstancingInstr.addImmediateOperand(compositionInstanceNum);

            shaderInstancingInstr.dump(bytecodeNewInstance->bytecode);
        }

        //apply the bytecode update controller
        if (!ApplyBytecodeUpdateController(bytecodeUpdateController)) error("failed to update the bytecode update controller");

        //update all maps
        if (!UpdateAllMaps()) return error("Failed to update all maps");
    }

    //===================================================================================================================
    //Add done
    if (errorMessages.size() > 0) return false;
    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}

//Find some composition instructions in the bytecode, in order to update it later
bool SpxCompiler::UpdateCompositionDataFromBytecodeForCompositionAndShaders(ShaderCompositionDeclaration* compositionToUpdate)
{
    spv::Id compositionShaderOwnerId = 0;
    int compositionShaderId = -1;

    if (compositionToUpdate != nullptr)
    {
        compositionToUpdate->tmpBytecodePosition = 0;
        compositionShaderOwnerId = compositionToUpdate->compositionShaderOwner->GetId();
        compositionShaderId = compositionToUpdate->compositionShaderId;
    }

    //reset all shaders listInstancingPaths
    for (auto its = vecAllShaders.begin(); its != vecAllShaders.end(); its++)
    {
        ShaderClassData* aShader = *its;
        aShader->listInstancingPathItems.clear();
    }

    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

        switch (opCode)
        {
            case spv::OpShaderCompositionDeclaration:
            {
                if (compositionToUpdate == nullptr) break;

                spv::Id shaderId = asId(start + 1);
                int compositionId = asLiteralValue(start + 2);
                if (shaderId == compositionShaderOwnerId && compositionId == compositionShaderId)
                {
                    compositionToUpdate->tmpBytecodePosition = start;
                }
                break;
            }

            case spv::OpShaderInstancingPathItem:
            {
                const spv::Id shaderId = asId(start + 1);

                ShaderClassData* shader = GetShaderById(shaderId);
                if (shader == nullptr) return error("undeclared shader id: " + to_string(shaderId));

                int instanceLevel = asLiteralValue(start + 2);
                const spv::Id shaderCompositionOwnerId = asId(start + 3);
                int compositionNum = asLiteralValue(start + 4);
                int instanceNum = asLiteralValue(start + 5);

                shader->listInstancingPathItems.push_back(ShaderInstancingPathItem(shaderId, instanceLevel, shaderCompositionOwnerId, compositionNum, instanceNum));

                break;
            }

            case spv::OpTypeVoid:
            case spv::OpTypeXlslShaderClass:
            {
                //can stop parsing the bytecode here
                start = end;
                break;
            }
        }

        start += wordCount;
    }

    if (compositionToUpdate != nullptr && compositionToUpdate->tmpBytecodePosition == 0) return error("Failed to update the composition bytecode position");
    //if (countInstanceLevelUpdated != countInstanceLevelToUpdate) return error("Failed to update the shaders instancing level data bytecode position");

    return true;
}

bool SpxCompiler::ApplyCompositionInstancesToBytecode()
{
    //if (status != SpxRemapperStatusEnum::AAA) return error("Invalid remapper status");
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

                ShaderCompositionDeclaration* compositionToUnroll = forEachLoopToUnroll->composition;
                if (compositionToUnroll->overridenBy != nullptr) compositionToUnroll = compositionToUnroll->overridenBy;

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
                        const unsigned int end = (unsigned int)foreachLoopBytecode.size();
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
                    unsigned int countInstances = (unsigned int)vecCompositionShaderInstances.size();
                    for (unsigned int instanceNum = 0; instanceNum < countInstances; ++instanceNum)
                    {
                        ShaderClassData* compositionShaderInstance = vecCompositionShaderInstances[instanceNum];

                        //Clone the loop bytecode at the end of the duplicated bytecode
                        {
                            vector<spv::Id> remapTable;
                            remapTable.resize(bound(), spvUndefinedId);
                            for (unsigned int id=0; id<IdsToRemapTable.size(); ++id) {
                                if (IdsToRemapTable[id]) remapTable[id] = maxId++;
                                else remapTable[id] = id;
                            }

                            unsigned int start = (unsigned int)duplicatedBytecode.size();
                            duplicatedBytecode.insert(duplicatedBytecode.end(), foreachLoopBytecode.begin(), foreachLoopBytecode.end());
                            const unsigned int end = (unsigned int)duplicatedBytecode.size();

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

                                        //Note: we don't refer to compositionToUnroll but use forEachLoopToUnroll->composition (compositionToUnroll could have been overriden by another composition)
                                        if (forEachLoopToUnroll->composition->compositionShaderOwner->GetId() == shaderId && forEachLoopToUnroll->composition->compositionShaderId == compositionId)
                                        {
                                            //in the case of the composition has been overriden
                                            duplicatedBytecode[start + 4] = compositionToUnroll->compositionShaderOwner->GetId();
                                            duplicatedBytecode[start + 5] = compositionToUnroll->compositionShaderId;

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
            unsigned int insertionPos = (unsigned int)spv.size();
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
        const unsigned int end = (unsigned int)spv.size();
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

                    ShaderCompositionDeclaration* composition = compositionShaderOwner->GetShaderCompositionDeclaration(compositionId);
                    if (composition == nullptr) { error(string("Shader: ") + compositionShaderOwner->GetName() + string(" has no composition for id: ") + to_string(compositionId)); break; }

                    if (composition->overridenBy != nullptr) composition = composition->overridenBy;

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

bool SpxCompiler::GetAllShaderInstancesForComposition(const ShaderCompositionDeclaration* composition, vector<ShaderClassData*>& instances)
{
    spv::Id compositionShaderOwnerId = composition->compositionShaderOwner->GetId();
    int compositionShaderId = composition->compositionShaderId;

    int countInstancesExpected = composition->countInstances;
    instances.clear();
    instances.resize(countInstancesExpected, nullptr);
    int countInstancesFound = 0;

    //look for the instances in the bytecode
    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
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

bool SpxCompiler::GetListAllCompositions(vector<ShaderCompositionDeclaration*>& vecCompositions)
{
    vecCompositions.clear();
    for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); ++it)
    {
        ShaderClassData* aShader = *it;
        unsigned int countCompositions = aShader->GetCountShaderComposition();
        for (unsigned int k = 0; k < countCompositions; ++k)
        {
            ShaderCompositionDeclaration* shaderComposition = aShader->listCompositionDeclarations[k];
            vecCompositions.push_back(shaderComposition);
        }
    }

    return true;
}

bool SpxCompiler::AddNewShaderCompositionDeclaration(ShaderClassData* shader, ShaderCompositionDeclaration* composition)
{
    shader->listCompositionDeclarations.push_back(composition);
    return true;
}

bool SpxCompiler::CheckIfTheCompositionGetOverridenByAnExistingStageComposition(ShaderCompositionDeclaration* newStagedComposition, vector<ShaderCompositionDeclaration*>& listStagedCompositionsPotentiallyOverriding)
{
    ShaderCompositionDeclaration* overridingComposition = nullptr;

    for (auto itc = listStagedCompositionsPotentiallyOverriding.begin(); itc != listStagedCompositionsPotentiallyOverriding.end(); itc++)
    {
        ShaderCompositionDeclaration* aPotentiallyOverridingComposition = *itc;

        if (newStagedComposition->isStage && aPotentiallyOverridingComposition->isStage &&
            newStagedComposition->isArray == aPotentiallyOverridingComposition->isArray &&
            newStagedComposition->compositionShaderOwner->GetShaderOriginalBaseName() == aPotentiallyOverridingComposition->compositionShaderOwner->GetShaderOriginalBaseName() &&
            newStagedComposition->shaderType->GetShaderOriginalBaseName() == aPotentiallyOverridingComposition->shaderType->GetShaderOriginalBaseName() &&
            newStagedComposition->variableName == aPotentiallyOverridingComposition->variableName
            )
        {
            overridingComposition = aPotentiallyOverridingComposition;
            break;
        }
    }

    if (overridingComposition == nullptr) return true; //no overriding composition found, can return

    //Is the overriding compositions overriden by another one?
    if (overridingComposition->overridenBy != nullptr)
    {
        overridingComposition = overridingComposition->overridenBy;
        bool found = false;

        //check that the overriding composition is in the list
        for (auto itc = listStagedCompositionsPotentiallyOverriding.begin(); itc != listStagedCompositionsPotentiallyOverriding.end(); itc++)
        {
            ShaderCompositionDeclaration* aComposition = *itc;
            if (aComposition == overridingComposition)
            {
                found = true;
                break;
            }
        }

        if (!found) return error("The overriding composition is overriden by a composition not found in our list of potential compositions");
    }

    //We can override the merged composition by the overriding one
    {
        newStagedComposition->overridenBy = overridingComposition;
        
        if (newStagedComposition->countInstances > 0)
        {
            return error("PROUT PROUT HERE");  //to update here !

            //the overriden composition already has some instances, we merged them into the overriding compositions
            vector<ShaderClassData*> vecCompositionShaderInstances;
            if (!GetAllShaderInstancesForComposition(newStagedComposition, vecCompositionShaderInstances)) {
                return error(string("Failed to retrieve the instances for the composition: ") + newStagedComposition->GetShaderOwnerAndVariableName());
            }

            BytecodeUpdateController bytecodeUpdateController;
            unsigned int countCompositionToTranfer = vecCompositionShaderInstances.size();
            for (unsigned int ict = 0; ict < countCompositionToTranfer; ict++)
            {
                ShaderClassData* compositionInstanceToTransfer = vecCompositionShaderInstances[ict];
                int compositionInstanceNum = overridingComposition->countInstances;
                overridingComposition->countInstances++;
                
                //if (!RecordNewInstanceForComposition(bytecodeUpdateController, overridingComposition, compositionInstanceToTransfer->GetId(), compositionInstanceNum, overridingComposition->countInstances))
                //    return error("Failed to insert a new composition into the bytecode");
            }

            //apply the bytecode update controller
            if (!ApplyBytecodeUpdateController(bytecodeUpdateController)) error("failed to update the bytecode update controller");

            //update all maps
            if (!UpdateAllMaps()) return error("Failed to update all maps");
        }
    }
       
    return true;
}

bool SpxCompiler::ValidateCompositionsAfterCompiling()
{
    vector<ShaderCompositionDeclaration*> vecAllCompositions;
    if (!GetListAllCompositions(vecAllCompositions))
        return error("Failed top get the list of all compositions");

    //check that no unstaged compositions have a name conflict with another one
    map<string, bool> mapName;
    for (auto itc = vecAllCompositions.begin(); itc != vecAllCompositions.end(); itc++)
    {
        ShaderCompositionDeclaration* anUnstagedComposition = *itc;
        if (anUnstagedComposition->isStage) continue;

        for (auto itc2 = vecAllCompositions.begin(); itc2 != vecAllCompositions.end(); itc2++)
        {
            ShaderCompositionDeclaration* anotherComposition = *itc2;
            if (anotherComposition == anUnstagedComposition) continue;

            const string& variableName = anUnstagedComposition->variableName;
            if (mapName.find(variableName) != mapName.end())
                return error("An unstaged compose variable has the same name as another existing compose variable: " + variableName);
            mapName[variableName] = true;
        }
    }

    return true;
}

//For all compositions merged, we check if they get overriden by another one
//A composition gets overriden if:
// - it is set as stage
// - it has the same shaderOwner name (original base name) and same shaderType (original base name)
// - it has the same variable name
//When a composition is overriden: everytime we instantiate it, we instantiate the overridding composition instead
//plus, when solving the composition function call, we will call the overriding composition instead
bool SpxCompiler::CheckIfAnyNewCompositionGetOverridenOrConflictsWithExistingOnes(vector<ShaderClassData*>& listMergedShaders)
{
    //vector<ShaderCompositionDeclaration*> listNewUnstagedCompositions;
    vector<ShaderCompositionDeclaration*> listNewStagedCompositions;
    vector<ShaderCompositionDeclaration*> listStagedCompositionsPotentiallyOverriding;

    //Reset all shaders flag
    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) {
        ShaderClassData* shader = *itsh;
        shader->flag1 = 0;
    }

    for (auto itsh = listMergedShaders.begin(); itsh != listMergedShaders.end(); itsh++) {
        ShaderClassData* shader = *itsh;
        shader->flag1 = 1;
    }
    //Get the list of all new and old compositions
    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++) {
        ShaderClassData* shader = *itsh;

        unsigned int countCompositions = shader->GetCountShaderComposition();
        for (unsigned int ic = 0; ic < countCompositions; ic++)
        {
            ShaderCompositionDeclaration* aComposition = shader->listCompositionDeclarations[ic];

#ifdef XKSLANG_DEBUG_MODE
            if (aComposition->compositionShaderOwner == nullptr) return error("The composition is missing link to its shader owner: " + aComposition->GetVariableName());
            if (aComposition->shaderType == nullptr) return error("The composition is missing link to its shader type: " + aComposition->GetVariableName());
#endif

            if (aComposition->isStage)
            {
                if (shader->flag1 == 1) listNewStagedCompositions.push_back(aComposition);
                else listStagedCompositionsPotentiallyOverriding.push_back(aComposition);
            }
            /*else
            {
                if (shader->flag1 == 1) listNewUnstagedCompositions.push_back(aComposition);
            }*/
        }
    }

    //check if some new, staged compositions get overriden by another one
    {
        if (listNewStagedCompositions.size() == 0 || listStagedCompositionsPotentiallyOverriding.size() == 0) return true;
        for (auto itc = listNewStagedCompositions.begin(); itc != listNewStagedCompositions.end(); itc++)
        {
            ShaderCompositionDeclaration* aNewStagedComposition = *itc;
            if (!CheckIfTheCompositionGetOverridenByAnExistingStageComposition(aNewStagedComposition, listStagedCompositionsPotentiallyOverriding))
                return error("Failed to check if the composition can be overriden: " + aNewStagedComposition->GetShaderOwnerAndVariableName());
        }
    }

    return true;
}

bool SpxCompiler::GetListAllCompositionsInfo(vector<ShaderCompositionInfo>& vecCompositionsInfo)
{
    vecCompositionsInfo.clear();

    vector<ShaderCompositionDeclaration*> vecCompositions;
    GetListAllCompositions(vecCompositions);

    for (auto it = vecCompositions.begin(); it != vecCompositions.end(); ++it)
    {
        ShaderCompositionDeclaration* shaderComposition = *it;
        string overridenBy = "";
        if (shaderComposition->overridenBy != nullptr)
        {
            overridenBy = shaderComposition->overridenBy->compositionShaderOwner->GetName() + "." + shaderComposition->overridenBy->GetVariableName();
        }

        vecCompositionsInfo.push_back(ShaderCompositionInfo(
            shaderComposition->compositionShaderOwner->GetName(),
            shaderComposition->shaderType->GetName(),
            shaderComposition->variableName,
            shaderComposition->isArray,
            shaderComposition->countInstances,
            overridenBy
        ));
    }

    return true;
}

bool SpxCompiler::GetAllCompositionsForVariableName(ShaderClassData* shader, const string& variableName, bool lookInParentShaders, vector<ShaderCompositionDeclaration*>& listCompositions)
{
#ifdef XKSLANG_DEBUG_MODE
    if (shader == nullptr) return error("Shader is null");
#endif

    for (auto itc = shader->listCompositionDeclarations.begin(); itc != shader->listCompositionDeclarations.end(); itc++)
    {
        ShaderCompositionDeclaration* aComposition = *itc;
        if (aComposition->variableName == variableName)
        {
            listCompositions.push_back(aComposition);
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

SpxCompiler::ShaderCompositionDeclaration* SpxCompiler::GetCompositionDeclaration(spv::Id shaderId, int compositionId)
{
    ShaderClassData* shader = GetShaderById(shaderId);
    if (shader == nullptr) return nullptr;
    return shader->GetShaderCompositionDeclaration(compositionId);
}

bool SpxCompiler::GetAllCompositionForEachLoops(vector<CompositionForEachLoopData>& vecForEachLoops, int& maxForEachLoopsNestedLevel)
{
    vecForEachLoops.clear();
    maxForEachLoopsNestedLevel = -1;

    int currentForEachLoopNestedLevel = -1;
    vector<CompositionForEachLoopData> pileForeachLoopsCurrentlyParsed;
    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
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

            ShaderCompositionDeclaration* composition = GetCompositionDeclaration(shaderId, compositionId);
            if (composition == nullptr) {
                error("Composition not found for ShaderId: " + to_string(shaderId) + " with composition num: " + to_string(compositionId));
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

SpxCompiler::ShaderCompositionDeclaration* SpxCompiler::GetShaderCompositionDeclarationForVariableName(ShaderClassData* shader, const string& variableName, bool lookInParentShaders)
{
    vector<ShaderCompositionDeclaration*> listCompositions;
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