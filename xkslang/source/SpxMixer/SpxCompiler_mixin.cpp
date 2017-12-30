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

//=====================================================================================================================================
//=====================================================================================================================================
void SpxCompiler::copyMessagesTo(vector<string>& list)
{
    list.insert(list.end(), errorMessages.begin(), errorMessages.end());
}

bool SpxCompiler::error(const string& txt)
{
    errorMessages.push_back(txt);
    return false;
}

bool SpxCompiler::error(vector<std::string>& errorMsgs, const string& txt)
{
    errorMsgs.push_back(txt);
    return false;
}

//=====================================================================================================================================
//=====================================================================================================================================
//static const auto spx_inst_fn_nop = [](spv::Op, unsigned) { return false; };
//static const auto spx_op_fn_nop = [](spv::Id&) {};

unsigned int SpxCompiler::currentMergeOperationId = 0;

unsigned int SpxCompiler::GetUniqueMergeOperationId()
{
    return SpxCompiler::currentMergeOperationId++;
}

void SpxCompiler::ResetMergeOperationId()
{
    SpxCompiler::currentMergeOperationId = 0;
}

spv::ExecutionModel SpxCompiler::GetExecutionModeForShadingStage(ShadingStageEnum stage)
{
    switch (stage) {
        case ShadingStageEnum::Vertex:           return spv::ExecutionModelVertex;
        case ShadingStageEnum::Pixel:            return spv::ExecutionModelFragment;
        case ShadingStageEnum::TessControl:      return spv::ExecutionModelTessellationControl;
        case ShadingStageEnum::TessEvaluation:   return spv::ExecutionModelTessellationEvaluation;
        case ShadingStageEnum::Geometry:         return spv::ExecutionModelGeometry;
        case ShadingStageEnum::Compute:          return spv::ExecutionModelGLCompute;
        default:                                 return spv::ExecutionModelMax;
    }
}

ShadingStageEnum SpxCompiler::GetShadingStageForExecutionMode(spv::ExecutionModel model)
{
    switch (model) {
        case spv::ExecutionModelVertex:                 return ShadingStageEnum::Vertex;        
        case spv::ExecutionModelFragment:               return ShadingStageEnum::Pixel;         
        case spv::ExecutionModelTessellationControl:    return ShadingStageEnum::TessControl;   
        case spv::ExecutionModelTessellationEvaluation: return ShadingStageEnum::TessEvaluation;
        case spv::ExecutionModelGeometry:               return ShadingStageEnum::Geometry;      
        case spv::ExecutionModelGLCompute:              return ShadingStageEnum::Compute;       
        default:                                        return ShadingStageEnum::Undefined;
    }
}

string SpxCompiler::getRawNameFromKeyName(const string& keyName)
{
    //return the raw name for the input keyName (for example rename Shader<8>_var will return Shader_8__var)
    string rawName = keyName;
    unsigned int len = (unsigned int)rawName.length();
    for (unsigned int k = 0; k < len; k++) {
        char c = rawName[k];
        if (c == '<' || c == '>' || c == ',' || c == '.') rawName[k] = '_';
    }
    return rawName;
}

SpxCompiler::VariableInstruction::~VariableInstruction()
{
    if (variableData != nullptr) 
    {
        bytecodeSource->error("unallocated variable Data");
    }
}

//=====================================================================================================================================

//int SpxCompiler::ShaderClassData::s_countRef = 0;
//int SpxCompiler::ShaderClassData::s_uniqueId = 0;

//=====================================================================================================================================
//=====================================================================================================================================

SpxCompiler::SpxCompiler(int verbose) : spirvbin_t(verbose)
{
    status = SpxRemapperStatusEnum::WaitingForMixin;
}

SpxCompiler::~SpxCompiler()
{
    ReleaseAllMaps();
}

void SpxCompiler::ReleaseAllMaps()
{
    unsigned int countObjects = (unsigned int)listAllObjects.size();
    for (unsigned int i = 0; i < countObjects; ++i)
    {
        if (listAllObjects[i] != nullptr) delete listAllObjects[i];
    }

    listAllObjects.clear();
    vecAllShaders.clear();
    vecAllFunctions.clear();
    mapDeclarationName.clear();
}

SpxCompiler* SpxCompiler::Clone()
{
    SpxCompiler* clonedSpxRemapper = new SpxCompiler();
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
                TypeInstruction* clonedType = dynamic_cast<TypeInstruction*>(clonedObj);

                if (type->GetTypePointed() != nullptr)
                {    
                    TypeInstruction* pointedType = clonedSpxRemapper->GetTypeById(type->GetTypePointed()->GetId());
                    clonedType->SetTypePointed(pointedType);
                }

                CBufferTypeData* typeCbData = type->GetCBufferData();
                if (typeCbData != nullptr && typeCbData->shaderOwner != nullptr)
                {
                    //update reference to the cbuffer data
                    clonedType->GetCBufferData()->shaderOwner = clonedSpxRemapper->GetShaderById(typeCbData->shaderOwner->GetId());
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
                    ShaderTypeData* clonedShaderType = new ShaderTypeData(clonedShader, clonedType, clonedTypePointer, clonedVariable);
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

    //add the shader, update the tmp references
    for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); it++)
    {
        ShaderClassData* shaderToClone = *it;
        ShaderClassData* clonedShader = clonedSpxRemapper->GetShaderById(shaderToClone->GetId());
        shaderToClone->tmpClonedShader = clonedShader;
        if (clonedShader == nullptr)
        {
            error("A shader has not been cloned");
            delete clonedSpxRemapper;
            return nullptr;
        }

        clonedSpxRemapper->vecAllShaders.push_back(clonedShader);
    }

    //clone all compositions
    {
        for (auto its = vecAllShaders.begin(); its != vecAllShaders.end(); its++)
        {
            ShaderClassData* shaderCloned = *its;
            ShaderClassData* clonedShader = shaderCloned->tmpClonedShader;

            unsigned int countCompositions = shaderCloned->GetCountShaderComposition();
            for (unsigned int k = 0; k < countCompositions; ++k)
            {
                ShaderCompositionDeclaration* compositionToClone = shaderCloned->listCompositionDeclarations[k];

                ShaderClassData* shaderType = compositionToClone->shaderType == nullptr ? nullptr : compositionToClone->shaderType->tmpClonedShader;
                ShaderClassData* shaderOwner = compositionToClone->compositionShaderOwner == nullptr ? nullptr : compositionToClone->compositionShaderOwner->tmpClonedShader;

                ShaderCompositionDeclaration* clonedComposition = new ShaderCompositionDeclaration(compositionToClone->compositionShaderNum, shaderOwner, shaderType,
                    compositionToClone->variableName, compositionToClone->isStage, compositionToClone->isArray, compositionToClone->countInstances);
                
                compositionToClone->tmpClonedComposition = clonedComposition;
                clonedSpxRemapper->AddNewShaderCompositionDeclaration(clonedShader, clonedComposition);
            }
        }
    }

    //update compositions references
    for (auto it = vecAllShaders.begin(); it != vecAllShaders.end(); it++)
    {
        ShaderClassData* shaderToClone = *it;
        ShaderClassData* clonedShader = shaderToClone->tmpClonedShader;
        unsigned int countCompositions = (unsigned int)(shaderToClone->listCompositionDeclarations.size());

        for (unsigned int i = 0; i < countCompositions; ++i)
        {
            ShaderCompositionDeclaration* compositionToClone = shaderToClone->listCompositionDeclarations[i];
            if (compositionToClone->overridenBy != nullptr)
            {
                ShaderCompositionDeclaration* overridingCompositionToClone = compositionToClone->overridenBy;
                if (overridingCompositionToClone->tmpClonedComposition == nullptr)
                {
                    error("A composition is missing its link to the cloned composition");
                    delete clonedSpxRemapper;
                    return nullptr;
                }

                ShaderCompositionDeclaration* clonedComposition = clonedShader->listCompositionDeclarations[i];
                clonedComposition->overridenBy = overridingCompositionToClone->tmpClonedComposition;
            }
        }
    }

    //clone the functions
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

SpxCompiler::FunctionInstruction* SpxCompiler::DuplicateFunctionBytecode(FunctionInstruction* functionToDuplicate, const std::string& functionNameSuffix)
{
    if (functionToDuplicate == nullptr) { error("null parameter: function to duplicate"); return nullptr; }

    spv::Id originalFunctionId = functionToDuplicate->GetId();
    spv::Id duplicatedFunctionId = 0;

    //Get the best position in the bytecode where we can insert the new function
    unsigned int posToInsertNewFunction = functionToDuplicate->bytecodeEndPosition;
    for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
        FunctionInstruction* aFunction = *itsf;
        if (aFunction->bytecodeEndPosition > posToInsertNewFunction)
            posToInsertNewFunction = aFunction->bytecodeEndPosition;
    }

    BytecodeUpdateController bytecodeUpdateController;
    BytecodeChunk* bytecodeDuplicatedFunction = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, posToInsertNewFunction, BytecodeChunkInsertionTypeEnum::InsertBeforeInstruction);
    vector<uint32_t>& duplicatedFunctionBytecode = bytecodeDuplicatedFunction->bytecode;

    //Clone the function bytecode
    duplicatedFunctionBytecode.insert(duplicatedFunctionBytecode.end(), spv.begin() + functionToDuplicate->bytecodeStartPosition, spv.begin() + functionToDuplicate->bytecodeEndPosition);

    int maxId = bound();

    //Will store all remapped resultId from the duplicated function
    vector<spv::Id> tableRemapId;
    tableRemapId.resize(maxId, 0);
    for (int k = 0; k < maxId; k++) tableRemapId[k] = k;

    //get the list of all resultIds to remap from the function bytecode
    {
        unsigned int start = 0;
        const unsigned int end = (unsigned int)duplicatedFunctionBytecode.size();
        while (start < end)
        {
            spv::Op opCode = spirvbin_t::opOpCode(duplicatedFunctionBytecode[start]);
            unsigned int wordCount = spirvbin_t::opWordCount(duplicatedFunctionBytecode[start]);
            unsigned int word = start + 1;

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) {error("Corrupted bytecode: wordCount is equals to 0"); return nullptr;}
#endif

            // any type?
            if (spv::InstructionDesc[opCode].hasType()) {
                word++;
            }

            // any result id to remap?
            if (spv::InstructionDesc[opCode].hasResult()) {
                spv::Id resultId = duplicatedFunctionBytecode[word];
#ifdef XKSLANG_DEBUG_MODE
                if (resultId < 0 || resultId >= tableRemapId.size()) {
                    error(string("duplicating function: result id is out of bound. Id: ") + to_string(resultId));
                    return nullptr;
                }
                if (tableRemapId[resultId] != resultId) {
                    error(string("duplicating function: a same result Id has been used several times. Id: ") + to_string(resultId));
                    return nullptr;
                }
#endif
                tableRemapId[resultId] = maxId++;
            }

            start += wordCount;
        }

        //remap all result Ids from the duplicated function bytecode
        if (!remapAllIds(duplicatedFunctionBytecode, 0, duplicatedFunctionBytecode.size(), tableRemapId))
        {
            error("remapAllIds failed on duplicatedBytecode");
            return nullptr;
        }

        if (originalFunctionId >= tableRemapId.size() || tableRemapId[originalFunctionId] < tableRemapId.size()) {
            error("the original function id has not been correctly remapped");
            return nullptr;
        }
        duplicatedFunctionId = tableRemapId[originalFunctionId];
    }

    //Duplicate the function's name & decorate instructions
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) { error("Corrupted bytecode: wordCount is equals to 0"); return nullptr; }
#endif

            switch (opCode)
            {
                case spv::OpName:
                case spv::OpDeclarationName:
                case spv::OpMethodProperties:
                case spv::OpGSMethodProperties:
                {
                    //We update the declaration name only for shader classes and for variables
                    const spv::Id id = asId(start + 1);

#ifdef XKSLANG_DEBUG_MODE
                    if (id >= tableRemapId.size()) { error("OpInstruction id is out of bound"); return nullptr; }
#endif
                    if (tableRemapId[id] != id)
                    {
                        spv::Id newId = tableRemapId[id];

                        BytecodeChunk* duplicatedInstruction = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, start, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);
                        if (duplicatedInstruction == nullptr) { error("Failed to create a new bytecode chunck"); return nullptr; }
                        vector<uint32_t>& duplicatedBytecode = duplicatedInstruction->bytecode;

                        if (newId == duplicatedFunctionId && (opCode == spv::OpName || opCode == spv::OpDeclarationName) && functionNameSuffix.size() > 0)
                        {
                            string name = literalString(start + 2);

                            //a function name is mangled with the parameters: we add the suffix after the function name but before the parameters
                            size_t index = name.find_first_of('(');
                            if (index != string::npos)
                                name = name.substr(0, index) + functionNameSuffix + name.substr(index);
                            else
                                name = name + functionNameSuffix;

                            //name of the duplicated function: we add the suffix
                            spv::Instruction functionNameInstr(opCode);
                            functionNameInstr.addIdOperand(newId);
                            functionNameInstr.addStringOperand(name.c_str());
                            functionNameInstr.dump(duplicatedBytecode);
                        }
                        else
                        {
                            duplicatedBytecode.insert(duplicatedBytecode.end(), spv.begin() + start, spv.begin() + start + wordCount);
                            duplicatedBytecode[1] = newId;
                        }
                    }
                    break;
                }

                case spv::OpBelongsToShader:
                {
                    //We update the declaration name only for shader classes and for variables
                    const spv::Id id = asId(start + 2);

#ifdef XKSLANG_DEBUG_MODE
                    if (id >= tableRemapId.size()) { error("OpInstruction id is out of bound"); return nullptr; }
#endif
                    if (tableRemapId[id] != id)
                    {
                        BytecodeChunk* duplicatedInstruction = CreateNewBytecodeChunckToInsert(bytecodeUpdateController, start, BytecodeChunkInsertionTypeEnum::InsertAfterInstruction);
                        if (duplicatedInstruction == nullptr) { error("Failed to create a new bytecode chunck"); return nullptr; }
                        vector<uint32_t>& duplicatedBytecode = duplicatedInstruction->bytecode;
                        duplicatedBytecode.insert(duplicatedBytecode.end(), spv.begin() + start, spv.begin() + start + wordCount);
                        duplicatedBytecode[2] = tableRemapId[id];
                    }
                    break;
                }
            }

            start += wordCount;
        }
    }

    //set the new Ids bound
    setBound(maxId);

    //apply the bytecode update controller
    if (!ApplyBytecodeUpdateController(bytecodeUpdateController)) { error("failed to update the bytecode update controller"); return nullptr; }
    ///spv.insert(spv.begin() + posToInsertNewFunction, duplicatedBytecode.begin(), duplicatedBytecode.end());

    //bytecode has been updated: reupdate all maps
    if (!UpdateAllMaps()) { error("Failed to update all maps"); return nullptr; }

    FunctionInstruction* duplicatedFunction = GetFunctionById(duplicatedFunctionId);
    if (duplicatedFunction == nullptr) { error("Failed to retrieve the duplicated function"); return nullptr; }
    return duplicatedFunction;
}

bool SpxCompiler::RemoveShaderTypeFromBytecodeAndData(ShaderTypeData* shaderTypeToRemove, vector<range_t>& vecStripRanges)
{
    ShaderClassData* shaderOwner = shaderTypeToRemove->shaderOwner;

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
            ShaderTypeData* shaderTypeToRemove = shaderOwner->shaderTypesList.back();
            shaderOwner->shaderTypesList.pop_back();
            delete shaderTypeToRemove;

            break;
        }
        if (!typeFound) return error("Failed to find the shaderType to remove: " + shaderTypeToRemove->type->GetName());
    }

    //remove all decorates related to the objects we removed
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

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
                case spv::OpShaderInstancingPathItem:
                case spv::OpBelongsToShader:
                case spv::OpMethodProperties:
                case spv::OpGSMethodProperties:
                case spv::OpMemberProperties:
                case spv::OpMemberAttribute:
                case spv::OpCBufferProperties:
                case spv::OpMemberSemanticName:
                case spv::OpSemanticName:
                case spv::OpMemberLinkName:
                case spv::OpMemberLogicalGroup:
                case spv::OpLinkName:
                case spv::OpResourceGroupName:
                case spv::OpLogicalGroupName:
                case spv::OpMemberSamplerStateDef:
                case spv::OpSamplerStateDef:
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

bool SpxCompiler::RemoveShaderFromBytecodeAndData(ShaderClassData* shaderToRemove, vector<range_t>& vecStripRanges)
{
    spv::Id shaderId = shaderToRemove->GetId();
    if (GetShaderById(shaderId) != shaderToRemove) return error("Failed to find the shader to remove: " + shaderToRemove->GetName());

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
    if (!shaderFound) return error("Failed to find the shader to remove: " + shaderToRemove->GetName());

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
            return error("Discrepancy between shader list of functions and list of all shader functions for shader: " + shaderToRemove->GetName());
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
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

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
                case spv::OpShaderInstancingPathItem:
                case spv::OpBelongsToShader:
                case spv::OpMethodProperties:
                case spv::OpGSMethodProperties:
                case spv::OpMemberProperties:
                case spv::OpMemberAttribute:
                case spv::OpCBufferProperties:
                case spv::OpMemberSemanticName:
                case spv::OpSemanticName:
                case spv::OpMemberLinkName:
                case spv::OpMemberLogicalGroup:
                case spv::OpLinkName:
                case spv::OpResourceGroupName:
                case spv::OpLogicalGroupName:
                case spv::OpMemberSamplerStateDef:
                case spv::OpSamplerStateDef:
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

bool SpxCompiler::MixWithShadersFromBytecode(const SpxBytecode& sourceBytecode, const vector<string>& nameOfShadersToMix)
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
    SpxCompiler bytecodeToMerge;
    if (!bytecodeToMerge.SetBytecode(sourceBytecode)) return error("Failed to set the bytecode");

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
    if (!SpxCompiler::GetShadersFullDependencies(&bytecodeToMerge, listShader, shadersFullDependencies)){
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
        return error("Failed to merge the shaders");
    }

    //===============================================================================================================================================
    //===============================================================================================================================================
    //retrieve the merged shaders from the destination bytecode
    vector<ShaderClassData*> listShadersMerged;
    for (unsigned int is = 0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is].shader;
        ShaderClassData* shaderMerged = shaderToMerge->tmpClonedShader; //GetShaderByName(shaderToMerge->GetName());
        if (shaderMerged == nullptr) return error(string("Cannot retrieve the merged shader: ") + shaderToMerge->GetName());
        listShadersMerged.push_back(shaderMerged);
    }

    if (!UpdateOverridingFunctionsAfterMixingNewShaders(listShadersMerged))
    {
        return error("Failed to update the overriding functions after having mixed some new shaders");
    }
    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}

bool SpxCompiler::UpdateOverridingFunctionsAfterMixingNewShaders(vector<ShaderClassData*>& listNewShaders)
{
    if (!ValidateSpxBytecodeAndData())
    {
        return error("Error validating the bytecode");
    }

    //=============================================================================================================
    //=============================================================================================================
    // Deal with base and overrides function

    //BEFORE checking for overrides, we first deal with unresolved base function calls
    if (!UpdateFunctionCallsHavingUnresolvedBaseAccessor())
    {
        return error("Updating function calls to base target failed");
    }

    //Update the list of overriding methods
    if (!UpdateOverridingFunctions(listNewShaders))
    {
        return error("Updating overriding functions failed");
    }

    //retarget the call to OpFunction instruction according to the overriding functions
    /*if (!UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions())
    {
        return error("Remapping overriding functions failed");
    }*/

    if (errorMessages.size() > 0) return false;
    return true;
}

void SpxCompiler::GetStagesPipeline(vector<ShadingStageEnum>& pipeline)
{
    pipeline = { ShadingStageEnum::Vertex, ShadingStageEnum::TessControl, ShadingStageEnum::TessEvaluation, ShadingStageEnum::Geometry , ShadingStageEnum::Pixel };
}

bool SpxCompiler::InitializeCompilationProcess(vector<XkslMixerOutputStage>& outputStages)
{
    //if (status != SpxRemapperStatusEnum::AAA) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_Initialized;

    if (outputStages.size() == 0) return error("no output stages defined");
    if (!ValidateHeader()) return error("Failed to validate the header");

#ifdef XKSLANG_DEBUG_MODE
    //check that the output stages are in the correct order
    vector<ShadingStageEnum> stagePipeline;
    SpxCompiler::GetStagesPipeline(stagePipeline);

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
            return error(string("The output stage is unknown, duplicated or not in the correct order: ") + GetShadingStageLabel(outputStage));
    }

#endif

    //===================================================================================================================
    //For each output stages, we search the entryPoint function in the bytecode
    for (unsigned int iStage = 0; iStage < outputStages.size(); iStage++)
    {
        XkslMixerOutputStage& outputStage = outputStages[iStage];
        FunctionInstruction* entryFunction = GetShaderFunctionForEntryPoint(outputStage.outputStage->entryPointName);

        if (entryFunction == nullptr) error(string("Entry point not found: ") + outputStage.outputStage->entryPointName);
        if (entryFunction->isEntryPointFunctionForStage != ShadingStageEnum::Undefined) error("The entry point function is used by more than one stage. Function: " + entryFunction->GetName());

        entryFunction->isEntryPointFunctionForStage = outputStage.outputStage->stage;
        outputStage.entryFunction = entryFunction;
    }
    if (errorMessages.size() > 0) return false;

    return true;
}

SpxCompiler::ConstInstruction* SpxCompiler::FindConstFromList(const vector<ConstInstruction*>& listConsts, spv::Op opCode, spv::Id typeId, const vector<unsigned int>& values)
{
    for (auto it = listConsts.begin(); it != listConsts.end(); it++)
    {
        ConstInstruction* aConst = *it;

        if (aConst->GetOpCode() == opCode && aConst->GetTypeId() == typeId)
        {
#ifdef XKSLANG_DEBUG_MODE
            //some sanity check
            if (asOpCode(aConst->GetBytecodeStartPosition()) != opCode) { error("Corrupted bytecode: the const opCode is not matching the bytecode"); return nullptr; }
#endif

            unsigned int wordCount = asWordCount(aConst->GetBytecodeStartPosition());
            unsigned int countValues = wordCount - 3;
            if (countValues == values.size())
            {
                bool gotIt = true;

                unsigned int startingPos = aConst->GetBytecodeStartPosition() + 3;
                for (unsigned int k = 0; k < countValues; ++k)
                {
                    if (asLiteralValue(startingPos + k) != values[k])
                    {
                        gotIt = false;
                        break;
                    }
                }

                if (gotIt)
                {
                    return aConst;
                }
            }
        }
    }

    return nullptr;
}

//parse an OpStruct instruction to get its list of membes typeId
bool SpxCompiler::GetStructTypeMembersTypeIdList(TypeInstruction* structType, vector<spv::Id>& membersTypeList)
{
#ifdef XKSLANG_DEBUG_MODE
    if (structType == nullptr) return error("null parameter");
#endif

    unsigned int start = structType->bytecodeStartPosition;
    unsigned int wordCount = asWordCount(start);

    spv::Op opCode = asOpCode(start);
    if (opCode != spv::OpTypeStruct) return error(string("Invalid OpCode for the stream struct type: ") + OpcodeString(opCode));

    int countMembers = wordCount - 2;
#ifdef XKSLANG_DEBUG_MODE
    if (countMembers < 0) return error("corrupted bytecode: countMembers is negative");
#endif

    for (int m = 0; m < countMembers; ++m)
    {
        spv::Id memberTypeId = asLiteralValue(start + 2 + m);
        membersTypeList.push_back(memberTypeId);

#ifdef XKSLANG_DEBUG_MODE
        if (GetTypeById(memberTypeId) == nullptr) return error("The struct member type is undefined: " + to_string(memberTypeId));
#endif
    }

    return true;
}

bool SpxCompiler::RemoveAllUnusedFunctionsAndMembers(vector<XkslMixerOutputStage>& outputStages)
{
	if (status != SpxRemapperStatusEnum::MixinBeingCompiled_CBuffersValidated) return error("Invalid remapper status");
	status = SpxRemapperStatusEnum::MixinBeingCompiled_UnusedStuffRemoved;

	vector<range_t> vecStripRanges;
	vector<bool> listIdsUsed;
	listIdsUsed.resize(bound(), false);

	//===================================================================================================================
	// Add some IDs we like to keep
	{
		unsigned int start = header_size;
		const unsigned int end = (unsigned int)spv.size();
		while (start < end)
		{
			unsigned int wordCount = asWordCount(start);
			spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

			switch (opCode)
			{
				case spv::OpExtInstImport:
				{
					spv::Id resultId = asId(start + 1);
					listIdsUsed[resultId] = true;
					break;
				}

				//can stop there
				case spv::OpName:
				case spv::OpConstant:
				case spv::OpTypeVoid:
				case spv::OpTypeFunction:
				{
					start = end;
					break;
				}
			}

			start += wordCount;
		}
	}

    int countFunctionsUsed = 0;
    int countFunctionsUnused = 0;
	//===================================================================================================================
	//Find all functions being called by the stages
	{
		//Set all functions flag to 0 (to check a function only once)
		for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
			FunctionInstruction* aFunction = *itsf;
			aFunction->flag1 = 0;
		}

		//add all entry funtions
		vector<FunctionInstruction*> vectorFunctionsToCheck;
		for (unsigned int i = 0; i < outputStages.size(); ++i)
		{
			XkslMixerOutputStage& stage = outputStages[i];
			FunctionInstruction* entryFunction = stage.entryFunction;
			if (entryFunction == nullptr) return error("The stage has no entry function");

			vectorFunctionsToCheck.push_back(stage.entryFunction);
			stage.entryFunction->flag1 = 1;
            countFunctionsUsed++;
		}

		//flag all functions called
		while (vectorFunctionsToCheck.size() > 0)
		{
			FunctionInstruction* aFunctionCalled = vectorFunctionsToCheck.back();
			vectorFunctionsToCheck.pop_back();

			//check if the function is calling any other function
			unsigned int start = aFunctionCalled->bytecodeStartPosition;
			const unsigned int end = aFunctionCalled->bytecodeEndPosition;
			while (start < end)
			{
				unsigned int wordCount = asWordCount(start);
				spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
                if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

				switch (opCode)
				{
					case spv::OpFunctionCall:
					case spv::OpFunctionCallBaseResolved:
					case spv::OpFunctionCallThroughStaticShaderClassCall:
					{
						//pile the function to go check it later
						spv::Id functionCalledId = asId(start + 3);
						FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
						if (anotherFunctionCalled == nullptr) return error("Failed to find the function for id: " + to_string(functionCalledId));
						if (anotherFunctionCalled->flag1 == 0) {
							anotherFunctionCalled->flag1 = 1;
							vectorFunctionsToCheck.push_back(anotherFunctionCalled); //we'll analyse the function later
                            countFunctionsUsed++;
						}
						break;
					}

#ifdef XKSLANG_DEBUG_MODE
					case spv::OpFunctionCallThroughCompositionVariable:
					case spv::OpFunctionCallBaseUnresolved:
					{
						return error(string("An unresolved function call has been found in function: ") + aFunctionCalled->GetFullName());
						break;
					}
#endif

				}
				start += wordCount;
			}
		}
	}

    countFunctionsUnused = (int)(vecAllFunctions.size()) - countFunctionsUsed;
#ifdef XKSLANG_DEBUG_MODE
    if (countFunctionsUnused < 0 || (unsigned int)countFunctionsUsed > vecAllFunctions.size()) return error("count functions: Internal error");
#endif

	//===================================================================================================================
	//Remove the unused functions
	{
		vector<FunctionInstruction*> listAllUsedFunctions;
        listAllUsedFunctions.reserve(countFunctionsUsed);
		vector<FunctionInstruction*> listAllUnusedFunctions;
        listAllUnusedFunctions.reserve(countFunctionsUnused);

		for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
			FunctionInstruction* aFunction = *itsf;
			if (aFunction->flag1 == 0) {
				listAllUnusedFunctions.push_back(aFunction);
			}
			else {
				listAllUsedFunctions.push_back(aFunction);
				listIdsUsed[aFunction->GetId()] = true;
			}
		}

		vecAllFunctions.clear();
		vecAllFunctions.insert(vecAllFunctions.end(), listAllUsedFunctions.begin(), listAllUsedFunctions.end());

		for (auto itsf = listAllUnusedFunctions.begin(); itsf != listAllUnusedFunctions.end(); itsf++)
		{
			FunctionInstruction* unusedFunction = *itsf;
			spv::Id functionId = unusedFunction->GetId();

#ifdef XKSLANG_DEBUG_MODE
			if (functionId >= listAllObjects.size() || listAllObjects[functionId] != unusedFunction) return error("Invalid function Id to remove: " + to_string(functionId));
#endif

			//The function could have declared some variable objects, check and delete them
			{
				unsigned int start = unusedFunction->bytecodeStartPosition;
				const unsigned int end = unusedFunction->bytecodeEndPosition;
				while (start < end)
				{
					unsigned int wordCount = asWordCount(start);
					spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
                    if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

					switch (opCode)
					{
						case spv::OpVariable:
						{
							const spv::Id variableId = asId(start + 2);
							ObjectInstructionBase* variableObj = listAllObjects[variableId];

							if (variableObj == nullptr) return error("Invalid function variable object for variableId: " + to_string(variableId));
							else
							{
								delete listAllObjects[variableId];
								listAllObjects[variableId] = nullptr;
							}
							break;
						}
					}
					start += wordCount;
				}
			}

			//delete the function object and its bytecode
			stripInst(vecStripRanges, unusedFunction->GetBytecodeStartPosition(), unusedFunction->GetBytecodeEndPosition());
			delete listAllObjects[functionId];
			listAllObjects[functionId] = nullptr;
		}
	}

	//===================================================================================================================
	//parse all used functions to get all IDs needed
	{
		for (auto itf = vecAllFunctions.begin(); itf != vecAllFunctions.end(); itf++)
		{
			FunctionInstruction* aFunctionCalled = *itf;

			spv::Op opCode;
			unsigned int wordCount;
			unsigned int start = aFunctionCalled->GetBytecodeStartPosition();
			const unsigned int end = aFunctionCalled->GetBytecodeEndPosition();
			while (start < end)
			{
				//add all IDs in the list of IDs to keep
				if (!flagAllIdsFromInstruction(start, opCode, wordCount, listIdsUsed))
					return error("Error flagging all IDs from the instruction");

				start += wordCount;
			}
		}

		//===================================================================================================================
		//For all IDs used: if it's defining a type, variable or const: check if it's depending on another type no included in the list yet
		{
			vector<spv::Id> idToCheckForExtraIds;
			for (unsigned int id = 0; id < listIdsUsed.size(); ++id)
			{
				if (listIdsUsed[id] == true)  //the id is used
				{
					if (listAllObjects[id] != nullptr)  //the id has been recognized as a data object
					{
						ObjectInstructionBase* obj = listAllObjects[id];
						switch (obj->GetKind())
						{
							case ObjectInstructionTypeEnum::Type:
							case ObjectInstructionTypeEnum::Variable:
							case ObjectInstructionTypeEnum::Const:
								idToCheckForExtraIds.push_back(id);
								break;
						}
					}
				}
			}

			spv::Op opCode;
			unsigned int wordCount;
			vector<spv::Id> listIds;
			spv::Id typeId, resultId;
			while (idToCheckForExtraIds.size() > 0)
			{
				spv::Id idToCheckForUnreferencedIds = idToCheckForExtraIds.back();
				idToCheckForExtraIds.pop_back();

				ObjectInstructionBase* obj = listAllObjects[idToCheckForUnreferencedIds];
				listIds.clear();
				if (!parseInstruction(obj->bytecodeStartPosition, opCode, wordCount, typeId, resultId, listIds))
					return error("Error parsing the instruction");

#ifdef XKSLANG_DEBUG_MODE
				if (resultId != idToCheckForUnreferencedIds) return error("The type, variable or const does not match its parsed instruction");  //sanity check
#endif

				if (typeId != spvUndefinedId && listIdsUsed[typeId] == false) listIds.push_back(typeId);
				for (unsigned int k = 0; k < listIds.size(); ++k)
				{
					const spv::Id& anotherId = listIds[k];
					if (listIdsUsed[anotherId] == false)
					{
						//we'll need this other id
						listIdsUsed[anotherId] = true;

						//If this other id is a type, const, or variable: we will need to check it as well
						if (listAllObjects[anotherId] != nullptr)
						{
							ObjectInstructionBase* anotherObj = listAllObjects[anotherId];
							switch (anotherObj->GetKind())
							{
								case ObjectInstructionTypeEnum::Type:
								case ObjectInstructionTypeEnum::Variable:
								case ObjectInstructionTypeEnum::Const:
									idToCheckForExtraIds.push_back(anotherId);
									break;
							}
						}
					}
				}
			}
		}
	}

	//remove all names and decorates related to the objects we want to remove
	{
		unsigned int start = header_size;
		const unsigned int end = (unsigned int)spv.size();
		while (start < end)
		{
			unsigned int wordCount = asWordCount(start);
			spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

			switch (opCode)
			{
				case spv::OpTypeXlslShaderClass:
				case spv::OpName:
				case spv::OpMemberName:
				case spv::OpDecorate:
				case spv::OpMemberDecorate:
				case spv::OpDeclarationName:
				case spv::OpShaderInheritance:
				case spv::OpShaderCompositionDeclaration:
				case spv::OpShaderCompositionInstance:
                case spv::OpShaderInstancingPathItem:
				case spv::OpBelongsToShader:
				case spv::OpMethodProperties:
				case spv::OpGSMethodProperties:
				case spv::OpMemberProperties:
				case spv::OpMemberAttribute:
				case spv::OpCBufferProperties:
				case spv::OpMemberSemanticName:
				case spv::OpSemanticName:
                case spv::OpMemberLinkName:
                case spv::OpMemberLogicalGroup:
                case spv::OpLinkName:
                case spv::OpResourceGroupName:
                case spv::OpLogicalGroupName:
                case spv::OpMemberSamplerStateDef:
                case spv::OpSamplerStateDef:
				{
					const spv::Id id = asId(start + 1);
					if (listIdsUsed[id] == false) {
						stripInst(vecStripRanges, start, start + wordCount);
					}
					break;
				}

				case spv::OpTypeVoid:
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

	//===================================================================================================================
	//Remove all unused types, consts and variables
	//We parse the bytecode and delete unused declarations
    int countShadersTypeRemoved = 0;
	{
		unsigned int start = header_size;
		const unsigned int end = (unsigned int)spv.size();
		while (start < end)
		{
			unsigned int wordCount = asWordCount(start);
			spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

			spv::Id id = spvUndefinedId;
			if (isConstOp(opCode))
			{
				id = asId(start + 2);
			}
			else if (isTypeOp(opCode))
			{
				id = asId(start + 1);
			}
			else if (isVariableOp(opCode))
			{
				id = asId(start + 2);
			}

			if (id != spvUndefinedId)
			{
#ifdef XKSLANG_DEBUG_MODE
				if (id >= listIdsUsed.size() || id >= listAllObjects.size()) return error("Invalid Id to remove: " + to_string(id));
#endif
				if (listIdsUsed[id] == false)
				{
					ObjectInstructionBase* obj = listAllObjects[id];
#ifdef XKSLANG_DEBUG_MODE
					if (obj == nullptr || obj->GetId() != id) return error("Invalid object Id to remove: " + to_string(id));
                    if (obj->GetKind() == ObjectInstructionTypeEnum::Function) return error("No unused functions should be parsed here");
#endif

                    if (obj->GetKind() == ObjectInstructionTypeEnum::Shader) countShadersTypeRemoved++;

					stripInst(vecStripRanges, obj->GetBytecodeStartPosition(), obj->GetBytecodeEndPosition());
					delete obj;
					listAllObjects[id] = nullptr;
				}
			}

			//stop at first OpFunction instruction
			if (opCode == spv::OpFunction) start = end;

			start += wordCount;
		}
	}

    //Remove the list of shaders
    if (countShadersTypeRemoved != vecAllShaders.size()) return error("We did not parse the correct number of shaders");
    vecAllShaders.clear();  //all shaders type have been set as "unused", and are now removed

    //can remove the link between data and shaders
    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj == nullptr) continue;
        obj->shaderOwner = nullptr;
    }

	stripBytecode(vecStripRanges);
	if (!UpdateAllMaps()) return error("failed to update all maps");

	//===================================================================================================================
	//defragment (remap) all IDs
	{
		//get the new IDs
		unsigned int newMaxId = 1;
		vector<spv::Id> remapTable;
		remapTable.resize(bound(), spvUndefinedId);
		for (spv::Id id = 0; id < listIdsUsed.size(); ++id)
		{
			if (listIdsUsed[id] == true) remapTable[id] = newMaxId++;
		}

		//Remap all Ids within the bytecode
		if (!remapAllIds(spv, header_size, (unsigned int)spv.size(), remapTable))
			return error("Failed to remap all IDs");

		//register the new Id into the bytecode
		setBound(spv, newMaxId);

		//remap and update the Ids for all internal objects
		std::vector<ObjectInstructionBase*> listAllObjectsRemapped;
		listAllObjectsRemapped.resize(bound(), nullptr);

		for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
		{
			ObjectInstructionBase* obj = *it;
			if (obj == nullptr) continue;

			if (obj->resultId != spvUndefinedId)
			{
#ifdef XKSLANG_DEBUG_MODE
				if (obj->resultId >= remapTable.size() || remapTable[obj->resultId] == spvUndefinedId || remapTable[obj->resultId] >= newMaxId) return error("Invalid value for remapping the object ids");
#endif
				obj->resultId = remapTable[obj->resultId];
			}

			if (obj->typeId != spvUndefinedId)
			{
#ifdef XKSLANG_DEBUG_MODE
				if (obj->typeId >= remapTable.size() || remapTable[obj->typeId] == spvUndefinedId || remapTable[obj->typeId] >= newMaxId) return error("Invalid value for remapping the object ids");
#endif
				obj->typeId = remapTable[obj->typeId];
			}

#ifdef XKSLANG_DEBUG_MODE
			if (listAllObjectsRemapped[obj->resultId] != nullptr) return error("2 objects are being remapped on the same Id");
#endif

			listAllObjectsRemapped[obj->resultId] = obj;
		}

		//switch the lists of objects
		listAllObjects = listAllObjectsRemapped;

		//remap outputStages pre-calculated IDs
		for (unsigned int i = 0; i<outputStages.size(); ++i)
		{
			XkslMixerOutputStage& stage = outputStages[i];
			for (unsigned int k = 0; k < stage.listStageInputVariableInfo.size(); k++)
				stage.listStageInputVariableInfo[k].spvVariableId = remapTable[stage.listStageInputVariableInfo[k].spvVariableId];
			for (unsigned int k = 0; k < stage.listStageOutputVariableInfo.size(); k++)
				stage.listStageOutputVariableInfo[k].spvVariableId = remapTable[stage.listStageOutputVariableInfo[k].spvVariableId];
		}
	}

	return true;
}

bool SpxCompiler::RemoveAllUnusedShaders(vector<XkslMixerOutputStage>& outputStages)
{
    return error("Obsolete function");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_UnusedStuffRemoved;

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

#ifdef XKSLANG_DEBUG_MODE
                if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

                switch (opCode)
                {
                    case spv::OpFunctionCall:
                    case spv::OpFunctionCallBaseResolved:
                    case spv::OpFunctionCallThroughStaticShaderClassCall:
                    {
                        spv::Id functionCalledId = asId(start + 3);
                        FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                        if (anotherFunctionCalled->flag1 == 0) vectorFunctionsCalled.push_back(anotherFunctionCalled); //we'll analyse the function later
                        break;
                    }

                    case spv::OpFunctionCallBaseUnresolved:
                    {
                        error("An unresolved function base call has been found in function: " + aFunctionCalled->GetFullName());
                        break;
                    }

                    case spv::OpFunctionCallThroughCompositionVariable:
                    {
                        error("An unresolved function call through composition has been found in function: " + aFunctionCalled->GetFullName());
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
            for (auto itc = aShaderInvolved->listCompositionDeclarations.begin(); itc != aShaderInvolved->listCompositionDeclarations.end(); itc++)
            {
                ShaderCompositionDeclaration* aComposition = *itc;
                if (aComposition->countInstances > 0)
                {
                    vector<ShaderClassData*> vecCompositionShaderInstances;
                    if (!GetAllShaderInstancesForComposition(aComposition, vecCompositionShaderInstances)) {
                        error(string("Failed to retrieve the instances for the composition: ") + aComposition->variableName + string(" from shader: ") + aComposition->compositionShaderOwner->GetName());
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

//For every call to a function using base accessor (base.function()), we check if we need to redirect to another overriding method
bool SpxCompiler::UpdateFunctionCallsHavingUnresolvedBaseAccessor()
{
    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

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
                    error("OpFunctionCallBaseUnresolved: targeted Id is not a known function. Id: " + to_string(functionCalledId));

                break;
            }
        }
        start += wordCount;
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxCompiler::UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions()
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_Initialized) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_OverridingMethodsProcessed;

    //reset flag
    for (auto itfn = vecAllFunctions.begin(); itfn != vecAllFunctions.end(); itfn++) (*itfn)->flag1 = 0;

    vector<FunctionInstruction*> vecFunctionIdBeingOverriden;
    vecFunctionIdBeingOverriden.resize(bound(), nullptr);
    bool anyOverridingFunction = false;
    //int currentFlag = 0; //used to detect circular chain between overriding functions
    for (auto itfn = vecAllFunctions.begin(); itfn != vecAllFunctions.end(); itfn++)
    {
        FunctionInstruction* function = *itfn;
        if (function->GetOverridingFunction() != nullptr && function->GetOverridingFunction() != function)
        {
            FunctionInstruction* overridingFunction = function->GetOverridingFunction();
            /*currentFlag++;
            overridingFunction->flag1 = currentFlag;

            while (overridingFunction->GetOverridingFunction() != nullptr && overridingFunction->GetOverridingFunction() != overridingFunction)
            {
                overridingFunction = overridingFunction->GetOverridingFunction();
                if (overridingFunction->flag1 == currentFlag) return error("Circular chain detected within overriding function links");
                overridingFunction->flag1 = currentFlag;
            }*/

            vecFunctionIdBeingOverriden[function->GetResultId()] = overridingFunction;
            anyOverridingFunction = true;
        }
    }

    if (!anyOverridingFunction) return true; //nothing to override

    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        switch (opCode)
        {
            // call to base function (OpFunctionCallBaseResolved, OpFunctionCallBaseUnresolved, OpFunctionCallThroughStaticShaderClassCall) are ignored

            case spv::OpFunctionCall:
            case spv::OpFunctionCallThroughCompositionVariable:
            {
                spv::Id functionCalledId = asId(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                if (functionCalledId < 0 || functionCalledId >= vecFunctionIdBeingOverriden.size()){
                    error("function call Id is out of bound. Id: " + to_string(functionCalledId));
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

void SpxCompiler::GetShaderChildrenList(ShaderClassData* shader, vector<ShaderClassData*>& children)
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
bool SpxCompiler::GetShadersFullDependencies(SpxCompiler* bytecodeSource, const vector<ShaderClassData*>& listShaders, vector<ShaderClassData*>& fullDependencies)
{
    fullDependencies.clear();

    //setup all shaders
    for (auto itsh = bytecodeSource->vecAllShaders.begin(); itsh != bytecodeSource->vecAllShaders.end(); itsh++){
        ShaderClassData* aShader = *itsh;
        aShader->flag = 0;
        aShader->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Undefined;
    }

    //setup list of initial shader
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
        for (auto itc = aShader->listCompositionDeclarations.begin(); itc != aShader->listCompositionDeclarations.end(); itc++)
        {
            ShaderCompositionDeclaration* aComposition = *itc;
            aComposition->shaderType->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
            if (aComposition->shaderType->flag == 0) listShaderToValidate.push_back(aComposition->shaderType);

            //add the composition instances
            if (aComposition->countInstances > 0)
            {
                vector<ShaderClassData*> vecCompositionShaderInstances;
                if (!bytecodeSource->GetAllShaderInstancesForComposition(aComposition, vecCompositionShaderInstances)) {
                    return bytecodeSource->error(string("Failed to retrieve the instances for the composition: ") + aComposition->variableName +
                        string(" from shader: ") + aComposition->compositionShaderOwner->GetName());
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

        //check the functions's bytecode, looking for function calls to static class, or for access to variables through static call (for example Global.Time)
        for (auto itf = aShader->functionsList.begin(); itf != aShader->functionsList.end(); itf++)
        {
            FunctionInstruction* aFunctionFromShader = *itf;
            
            unsigned int start = aFunctionFromShader->bytecodeStartPosition;
            const unsigned int end = aFunctionFromShader->bytecodeEndPosition;
            while (start < end)
            {
                unsigned int wordCount = bytecodeSource->asWordCount(start);
                spv::Op opCode = bytecodeSource->asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
                if (wordCount == 0) return bytecodeSource->error("Corrupted bytecode: wordCount is equals to 0");
#endif

                switch (opCode)
                {
                    case spv::OpFunctionCall:
                    case spv::OpFunctionCallBaseResolved:
                    case spv::OpFunctionCallBaseUnresolved:
                    case spv::OpFunctionCallThroughStaticShaderClassCall:
                    case spv::OpFunctionCallThroughCompositionVariable:
                    {
                        spv::Id functionCalledId = bytecodeSource->asId(start + 3);
                        FunctionInstruction* functionCalled = bytecodeSource->GetFunctionById(functionCalledId);
                        if (functionCalled == nullptr) {
                            return bytecodeSource->error("Failed to retrieve the function for Id: " + to_string(functionCalledId));
                        }

                        ShaderClassData* functionShaderOwner = functionCalled->shaderOwner;
                        if (functionShaderOwner != nullptr && functionShaderOwner->flag == 0)
                        {
                            //if we're calling a static function, set the dependency type to static (unless it was already set to another dependency type)
                            if (functionCalled->IsStatic() && functionShaderOwner->dependencyType == ShaderClassData::ShaderDependencyTypeEnum::Undefined)
                                functionShaderOwner->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::StaticAccess;
                            else
                                functionShaderOwner->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
                            listShaderToValidate.push_back(functionShaderOwner);
                        }

                        break;
                    }

                    case spv::OpAccessChain:
                    {
                        spv::Id structIdAccessed = bytecodeSource->asId(start + 3);
                        VariableInstruction* variable = bytecodeSource->GetVariableById(structIdAccessed);
                        //if (variable == nullptr) { error("Failed to find the variable object for id: " + to_string(structIdAccessed)); break; }

                        //are we accessing the a cbuffer variable
                        if (variable != nullptr)
                        {
                            ShaderClassData* shaderOwner = variable->shaderOwner;
                            if (shaderOwner != nullptr && shaderOwner->flag == 0)
                            {
                                //if we're accessing a variable through static call, set the dependency type to StaticVariableAccess (unless it was already set to another dependency type)
                                if (shaderOwner->dependencyType == ShaderClassData::ShaderDependencyTypeEnum::Undefined)
                                    shaderOwner->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::StaticAccess;
                                else
                                    shaderOwner->dependencyType = ShaderClassData::ShaderDependencyTypeEnum::Other;
                                listShaderToValidate.push_back(shaderOwner);
                            }
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

static bool shaderLevelSortFunction(SpxCompiler::ShaderClassData* sa, SpxCompiler::ShaderClassData* sb)
{
    return (sa->level < sb->level);
}

bool SpxCompiler::UpdateOverridingFunctions(vector<ShaderClassData*>& listShadersMerged)
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

                        //Both functions must have the same attributes / qualifier (such as stage)
                        if (aFunction->isStage != overridingFunction->isStage)
                        {
                            if (overridingFunction->isStage) return error("A stage method is overriding an unstage method: " + overringFunctionName);
                            else return error("An unstage method is overriding a stage method: " + overringFunctionName);
                        }

                        aFunction->SetOverridingFunction(overridingFunction);
                    }
                }
            }
        }
    }

    if (errorMessages.size() > 0) return false;

    return true;
}

const vector<std::uint32_t>& SpxCompiler::GetMixinBytecode()
{
    return spv;
}

void SpxCompiler::CopyMixinBytecode(vector<uint32_t>& bytecodeStream)
{
    bytecodeStream.clear();
    bytecodeStream.insert(bytecodeStream.end(), spv.begin(), spv.end());
}

bool SpxCompiler::GetFunctionLabelInstructionPosition(FunctionInstruction* function, unsigned int& labelPos)
{
    unsigned int start = function->bytecodeStartPosition;
    const unsigned int end = function->bytecodeEndPosition;
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        switch (opCode)
        {
            case spv::OpLabel:
            {
                labelPos = start;
                return true;
            }
        }

        start += wordCount;
    }

    return false;
}

bool SpxCompiler::GetFunctionLabelAndReturnInstructionsPosition(FunctionInstruction* function, unsigned int& labelPos, unsigned int& latestReturnPos, unsigned int& countReturnInstructions)
{
    labelPos = 0;
    latestReturnPos = 0;
    countReturnInstructions = 0;

    int countInstructions = 0;
    unsigned int start = function->bytecodeStartPosition;
    const unsigned int end = function->bytecodeEndPosition;
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        countInstructions++;
        switch (opCode)
        {
            case spv::OpLabel:
            {
                //The first instruction of a function (after OpFunction) is OpLabel
                if (countInstructions == 2) labelPos = start;
                break;
            }
            case spv::OpReturn:
            case spv::OpReturnValue:
            {
                countReturnInstructions++;
                latestReturnPos = start;
                break;
            }
        }

        start += wordCount;
    }

    if (labelPos == 0) return error("Failed to find the function OpLabel instruction");
    return true;
}

SpxCompiler::FunctionInstruction* SpxCompiler::GetShaderFunctionForEntryPoint(string entryPointName)
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

bool SpxCompiler::FinalizeCompilation(vector<XkslMixerOutputStage>& outputStages)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_UnusedStuffRemoved) return error("Invalid remapper status");
    status = SpxRemapperStatusEnum::MixinBeingCompiled_Finalized;

    //===================================================================================================================
    //Convert SPIRX to SPIRV (remove all SPIRX extended instructions). So we don't need to repeat it for every stages
    //Some exceptions, we keep the instructions:
    // -OpSemanticName: to let SPIRV-Cross set the semantic name as it was defined by the user in the original xksl shaders
    // -OpMemberAttribute: to let us correctly process the types reflection, for example with attribute like "[Color]"
    vector<range_t> vecStripRanges;
    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        switch (opCode)
        {
            case spv::OpEntryPoint:
            {
                //remove all current entry points (if there is any): we will set a new one for each stages
                stripInst(vecStripRanges, start);
                break;
            }
            case spv::OpFunctionCallBaseResolved:
            case spv::OpFunctionCallThroughStaticShaderClassCall:
            {
                //change OpCode to OpFunctionCall
                setOpCode(start, spv::OpFunctionCall);
                break;
            }
            case spv::OpFunctionCallThroughCompositionVariable:
            {
                error("Found unresolved OpFunctionCallThroughCompositionVariable at: " + to_string(start));
                break;
            }
            case spv::OpFunctionCallBaseUnresolved:
            {
                error("A function base call has an unresolved state at: " + to_string(start));
                break;
            }
            case spv::OpForEachCompositionStartLoop:
            {
                error("A foreach start loop has not been processed at: " + to_string(start));
                break;
            }
            case spv::OpForEachCompositionEndLoop:
            {
                error("A foreach end loop has not been processed at: " + to_string(start));
                break;
            }
            case spv::OpBelongsToShader:
            case spv::OpDeclarationName:
            case spv::OpTypeXlslShaderClass:
            case spv::OpShaderInheritance:
            case spv::OpShaderCompositionDeclaration:
            case spv::OpShaderCompositionInstance:
            case spv::OpShaderInstancingPathItem:
            case spv::OpMethodProperties:
            case spv::OpGSMethodProperties:
            case spv::OpMemberProperties:
            case spv::OpMemberSemanticName:
            case spv::OpCBufferProperties:
            case spv::OpMemberSamplerStateDef:
            {
                stripInst(vecStripRanges, start);
                break;
            }

            //Keep the following SPX extensions:
            //case spv::OpSemanticName:
            //case spv::OpSamplerStateDef:
            //case spv::OpMemberAttribute:
            //case spv::OpMemberLinkName:
            //case spv::OpMemberLogicalGroup:
            //case spv::OpLinkName:
            //case spv::OpResourceGroupName:
            //case spv::OpLogicalGroupName:
            //{
            //}
        }
        start += wordCount;
    }

    if (errorMessages.size() > 0) return false;
    stripBytecode(vecStripRanges);

    //============================================================================================================
    //Add the entry points for each output stages
    vector<uint32_t> entryPointInstructions;
    for (unsigned int i = 0; i<outputStages.size(); ++i)
    {
        XkslMixerOutputStage& stage = outputStages[i];
        FunctionInstruction* entryFunction = stage.entryFunction;
        if (entryFunction == nullptr) return error("The stage has no entry function");

        spv::ExecutionModel model = GetExecutionModeForShadingStage(stage.outputStage->stage);
        if (model == spv::ExecutionModelMax) return error("Unknown stage");
        spv::Instruction entryPointInstr(spv::OpEntryPoint);
        entryPointInstr.addImmediateOperand(model);
        entryPointInstr.addIdOperand(entryFunction->GetResultId());
        entryPointInstr.addStringOperand(stage.outputStage->entryPointName.c_str());
        //Add all input variables Ids
        for (unsigned int k = 0; k < stage.listStageInputVariableInfo.size(); k++)
            entryPointInstr.addIdOperand(stage.listStageInputVariableInfo[k].spvVariableId);
        //Add all output variables Ids
        for (unsigned int k = 0; k < stage.listStageOutputVariableInfo.size(); k++)
            entryPointInstr.addIdOperand(stage.listStageOutputVariableInfo[k].spvVariableId);
        entryPointInstr.dump(entryPointInstructions);
    }
    spv.insert(spv.begin() + header_size, entryPointInstructions.begin(), entryPointInstructions.end());

    //============================================================================================================
    if (!UpdateAllMaps()) return error("RemoveAndConvertSPXExtensions: failed to update all maps");

    if (errorMessages.size() > 0) return false;
    return true;
}

//Mixin is finalized: no more updates will be brought to the mixin bytecode after
bool SpxCompiler::GenerateBytecodeForAllStages(vector<XkslMixerOutputStage>& outputStages)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_Finalized) return error("Invalid remapper status");

    if (outputStages.size() == 0) return error("no output stages defined");

    //===================================================================================================================
    //===================================================================================================================
    //Get the IDs of all cbuffers (plus their pointer and variable types)
    //We keep all cbuffers, even if some are not used by the output stage (unused cbuffers have already been removed)
    vector<spv::Id> listCBufferIds;

    /*
    {
        for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
        {
            ObjectInstructionBase* obj = *it;
            if (obj == nullptr) continue;

            switch (obj->GetKind())
            {
                case ObjectInstructionTypeEnum::Type:
                {
                    TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
                    if (type->IsCBuffer()) listCBufferIds.push_back(type->GetId());
                    else
                    {
                        if (type->pointerTo != nullptr && type->pointerTo->IsCBuffer()) listCBufferIds.push_back(type->GetId());
                    }
                    break;
                }

                case ObjectInstructionTypeEnum::Variable:
                {
                    VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
                    if (variable->variableTo != nullptr && variable->variableTo->pointerTo != nullptr && variable->variableTo->pointerTo->IsCBuffer())
                        listCBufferIds.push_back(variable->GetId());
                }
            }
        }
    }
    */

    //===================================================================================================================
    // Generate the SPIRV bytecode for all stages
    for (unsigned int i = 0; i<outputStages.size(); ++i)
    {
        XkslMixerOutputStage& outputStage = outputStages[i];

        if (!GenerateBytecodeForStage(outputStage, listCBufferIds))
        {
            error("Failed to set and clean the stage bytecode");
            outputStage.outputStage->resultingBytecode.clear();
            break;
        }
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxCompiler::GenerateBytecodeForStage(XkslMixerOutputStage& stage, vector<spv::Id>& listObjectIdsToKeep)
{
    if (status != SpxRemapperStatusEnum::MixinBeingCompiled_Finalized) return error("Invalid remapper status");
    FunctionInstruction* entryFunction = stage.entryFunction;
    if (entryFunction == nullptr) return error("The stage has no entry function");

    //Find the output stage entryPoint
    vector<uint32_t> stageEntryPointInstr;
    {
        spv::ExecutionModel outputModel = GetExecutionModeForShadingStage(stage.outputStage->stage);
        if (outputModel == spv::ExecutionModelMax) return error("Unknown stage");

        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

            //By default: copy memoryModel and external import (this might change later depending on the stage)
            switch (opCode)
            {
                case spv::OpEntryPoint:
                {
                    spv::ExecutionModel model = (spv::ExecutionModel)asLiteralValue(start + 1);
                    if (model == outputModel)
                    {
                        stageEntryPointInstr.insert(stageEntryPointInstr.begin(), spv.begin() + start, spv.begin() + start + wordCount);
                        start = end;
                    }
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
        if (stageEntryPointInstr.size() == 0) return error("Failed to find the stage's entry point instruction");
    }

    //======================================================================================================
    //Find all functions being called by the stage
    vector<FunctionInstruction*> listAllFunctionsCalledByTheStage;
    {
        //Set all functions flag to 0 (to check a function only once)
        for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
            FunctionInstruction* aFunction = *itsf;
            aFunction->flag1 = 0;
        }

        vector<FunctionInstruction*> vectorFunctionsToCheck;
        vectorFunctionsToCheck.push_back(stage.entryFunction);
        stage.entryFunction->flag1 = 1;
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

#ifdef XKSLANG_DEBUG_MODE
                if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

                switch (opCode)
                {
                    case spv::OpFunctionCall:
                    {
                        //pile the function to go check it later
                        spv::Id functionCalledId = asId(start + 3);
                        FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                        if (anotherFunctionCalled == nullptr) return error("Failed to find the function for id: " + to_string(functionCalledId));
                        if (anotherFunctionCalled->flag1 == 0) {
                            anotherFunctionCalled->flag1 = 1;
                            vectorFunctionsToCheck.push_back(anotherFunctionCalled); //we'll analyse the function later
                        }
                        break;
                    }

                    case spv::OpFunctionCallBaseResolved:
                    case spv::OpFunctionCallThroughStaticShaderClassCall:
                    case spv::OpFunctionCallThroughCompositionVariable:
                    case spv::OpFunctionCallBaseUnresolved:
                    {
                        return error(string("An unresolved function call has been found in function: ") + aFunctionCalled->GetFullName());
                        break;
                    }
                }
                start += wordCount;
            }
        }
    }

    //======================================================================================================
    //Init the list of IDs that we need to keep
    vector<bool> listIdsUsed;
    listIdsUsed.resize(bound(), false);

    //Keep some IDs
    for (auto it = listObjectIdsToKeep.begin(); it != listObjectIdsToKeep.end(); it++)
    {
        const spv::Id& id = *it;
#ifdef XKSLANG_DEBUG_MODE
        if (id >= listIdsUsed.size()) return error("An Id is out of bound");
#endif
        listIdsUsed[id] = true;
    }

    //======================================================================================================
    //======================================================================================================
    //Build the header
    vector<uint32_t> header;
    header.insert(header.begin(), spv.begin(), spv.begin() + header_size);

    //======================================================================================================
    //Add stage capabilities and execution mode (cf TGlslangToSpvTraverser::TGlslangToSpvTraverser function)
    vector<spv::Capability> capabilities;
    vector<SPVHeaderStageExecutionMode> executionModes;
    {
        switch (stage.outputStage->stage)
        {
            case ShadingStageEnum::Vertex:
                capabilities.push_back(spv::CapabilityShader);
                break;
            case ShadingStageEnum::Geometry:
                capabilities.push_back(spv::CapabilityGeometry);
                return error("stage not processed yet"); //(cf TGlslangToSpvTraverser::TGlslangToSpvTraverser function)
                break;
            case ShadingStageEnum::TessControl:
                capabilities.push_back(spv::CapabilityTessellation);
                return error("stage not processed yet"); //(cf TGlslangToSpvTraverser::TGlslangToSpvTraverser function)
                break;
            case ShadingStageEnum::TessEvaluation:
                capabilities.push_back(spv::CapabilityTessellation);
                return error("stage not processed yet"); //(cf TGlslangToSpvTraverser::TGlslangToSpvTraverser function)
                break;
            case ShadingStageEnum::Pixel:
                capabilities.push_back(spv::CapabilityShader);
                executionModes.push_back(SPVHeaderStageExecutionMode(spv::ExecutionModeOriginUpperLeft));  //default for now!!
                break;
            case ShadingStageEnum::Compute:
                capabilities.push_back(spv::CapabilityShader);
                return error("stage not processed yet"); //(cf TGlslangToSpvTraverser::TGlslangToSpvTraverser function)
                break;
            default:
                return error("unknown stage");
        }
        for (auto itc = capabilities.begin(); itc != capabilities.end(); itc++)
        {
            const spv::Capability& capability = *itc;
            spv::Instruction capInst(0, 0, spv::OpCapability);
            capInst.addImmediateOperand(capability);
            capInst.dump(header);
        }
    }

    //Add the entry point instruction
    header.insert(header.end(), stageEntryPointInstr.begin(), stageEntryPointInstr.end());

    //add the execution modes
    for (auto itc = executionModes.begin(); itc != executionModes.end(); itc++)
    {
        const SPVHeaderStageExecutionMode& execMode = *itc;
        spv::Instruction instrExecMode(spv::OpExecutionMode);
        instrExecMode.addIdOperand(entryFunction->GetResultId());
        instrExecMode.addImmediateOperand(execMode.mode);
        if (execMode.value1 >= 0) instrExecMode.addImmediateOperand(execMode.value1);
        if (execMode.value2 >= 0) instrExecMode.addImmediateOperand(execMode.value2);
        if (execMode.value3 >= 0) instrExecMode.addImmediateOperand(execMode.value3);
        instrExecMode.dump(header);
    }

    //======================================================================================================
    //======================================================================================================
    // Add some header data from original spv files
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

            //By default: copy memoryModel and external import (this might change later depending on the stage)
            switch (opCode)
            {
                case spv::OpMemoryModel:
                {
                    header.insert(header.end(), spv.begin() + start, spv.begin() + (start + wordCount));
                    break;
                }
                case spv::OpExtInstImport:
                {
                    spv::Id resultId = asId(start + 1);
                    listIdsUsed[resultId] = true;
                    header.insert(header.end(), spv.begin() + start, spv.begin() + (start + wordCount));
                    break;
                }

                case spv::OpName:
                case spv::OpMemberName:
                case spv::OpFunction:
                case spv::OpConstant:
                case spv::OpTypeInt:
                {
                    start = end;
                    break;
                }
            }

            start += wordCount;
        }
    }

    //======================================================================================================
    //parse all functions called by the stage to get all IDs needed
    for (auto itf = listAllFunctionsCalledByTheStage.begin(); itf != listAllFunctionsCalledByTheStage.end(); itf++)
    {
        FunctionInstruction* aFunctionCalled = *itf;

        spv::Op opCode;
        unsigned int wordCount;
        unsigned int start = aFunctionCalled->GetBytecodeStartPosition();
        const unsigned int end = aFunctionCalled->GetBytecodeEndPosition();
        while (start < end)
        {
            //add all IDs in the list of IDs to keep
            if (!flagAllIdsFromInstruction(start, opCode, wordCount, listIdsUsed))
                return error("Error flagging all IDs from the instruction");
        
            start += wordCount;
        }
    }

    //======================================================================================================
    //For all IDs used: if it's defining a type, variable or const: check if it's depending on another type no included in the list yet
    {
        vector<spv::Id> idToCheckForExtraIds;
        for (unsigned int id = 0; id < listIdsUsed.size(); ++id)
        {
            if (listIdsUsed[id] == true)  //the id is used
            {
                if (listAllObjects[id] != nullptr)  //the id has been recognized as a data object
                {
                    ObjectInstructionBase* obj = listAllObjects[id];
                    switch (obj->GetKind())
                    {
                        case ObjectInstructionTypeEnum::Type:
                        case ObjectInstructionTypeEnum::Variable:
                        case ObjectInstructionTypeEnum::Const:
                            idToCheckForExtraIds.push_back(id);
                            break;
                    }
                }
            }
        }

        spv::Op opCode;
        unsigned int wordCount;
        vector<spv::Id> listIds;
        spv::Id typeId, resultId;
        while (idToCheckForExtraIds.size() > 0)
        {
            spv::Id idToCheckForUnreferencedIds = idToCheckForExtraIds.back();
            idToCheckForExtraIds.pop_back();

            ObjectInstructionBase* obj = listAllObjects[idToCheckForUnreferencedIds];
            listIds.clear();
            if (!parseInstruction(obj->bytecodeStartPosition, opCode, wordCount, typeId, resultId, listIds))
                return error("Error parsing the instruction");

#ifdef XKSLANG_DEBUG_MODE
            if (resultId != idToCheckForUnreferencedIds) return error("The type, variable or const does not match its parsed instruction");  //sanity check
#endif

            if (typeId != spvUndefinedId && listIdsUsed[typeId] == false) listIds.push_back(typeId);
            for (unsigned int k = 0; k < listIds.size(); ++k)
            {
                const spv::Id& anotherId = listIds[k];
                if (listIdsUsed[anotherId] == false)
                {
                    //we'll need this other id
                    listIdsUsed[anotherId] = true;
                    
                    //If this other id is a type, const, or variable: we will need to check it as well
                    if (listAllObjects[anotherId] != nullptr)
                    {
                        ObjectInstructionBase* anotherObj = listAllObjects[anotherId];
                        switch (anotherObj->GetKind())
                        {
                            case ObjectInstructionTypeEnum::Type:
                            case ObjectInstructionTypeEnum::Variable:
                            case ObjectInstructionTypeEnum::Const:
                                idToCheckForExtraIds.push_back(anotherId);
                                break;
                        }
                    }
                }
            }
        }
    }

    //======================================================================================================
    //copy the header
    vector<uint32_t>& stageBytecode = stage.outputStage->resultingBytecode.getWritableBytecodeStream();
    stageBytecode.clear();
    stageBytecode.insert(stageBytecode.end(), header.begin(), header.end());

#ifdef XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE
    //======================================================================================================
    //copy all names and decorate matching our IDS
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

            switch (opCode)
            {
                case spv::OpName:
                case spv::OpMemberName:
                case spv::OpDecorate:
                case spv::OpMemberDecorate:
                case spv::OpSemanticName: //keep the semantics name (so that we can convert it back with the initial semantic)
                {
                    const spv::Id id = asId(start + 1);
                    if (listIdsUsed[id] == true)
                    {
                        auto its = spv.begin() + start;
                        stageBytecode.insert(stageBytecode.end(), its, its + wordCount);
                    }
                    break;
                }
                
                case spv::OpTypeInt:
                case spv::OpTypeVoid:
                case spv::OpTypePointer:
                case spv::OpFunction:
                {
                    //can stop parsing at this point
                    start = end;
                    break;
                }
            }

            start += wordCount;
        }
    }
#endif

    //======================================================================================================
    //Copy all types matching our IDS
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

            spv::Id resultId = spvUndefinedId;
            if (isConstOp(opCode))
            {
                resultId = asId(start + 2);
            }
            else if (isTypeOp(opCode))
            {
                resultId = asId(start + 1);
            }
            else if (isVariableOp(opCode))
            {
                resultId = asId(start + 2);
            }

            if (resultId != spvUndefinedId)
            {
                if (listIdsUsed[resultId] == true)
                {
                    auto its = spv.begin() + start;
                    stageBytecode.insert(stageBytecode.end(), its, its + wordCount);
                }
            }

            //stop at first OpFunction instruction
            if (opCode == spv::OpFunction) start = end;
    
            start += wordCount;
        }
    }

    //======================================================================================================
    //copy all functions' bytecode
    for (auto itf = listAllFunctionsCalledByTheStage.begin(); itf != listAllFunctionsCalledByTheStage.end(); itf++)
    {
        FunctionInstruction* aFunctionCalled = *itf;
        stageBytecode.insert(stageBytecode.end(), spv.begin() + aFunctionCalled->GetBytecodeStartPosition(), spv.begin() + aFunctionCalled->GetBytecodeEndPosition());
    }

    //======================================================================================================
    //Remap all IDs
    {
        int newId = 1;
        vector<spv::Id> remapTable;
        remapTable.resize(bound(), spvUndefinedId);
        for (spv::Id id = 0; id < listIdsUsed.size(); ++id)
        {
            if (listIdsUsed[id] == true) remapTable[id] = newId++;
        }

        if (!remapAllIds(stageBytecode, header_size, (unsigned int)stageBytecode.size(), remapTable))
            return error("Failed to remap all IDs");

        setBound(stageBytecode, newId);
    }

    //======================================================================================================
    //Finalize the stage bytecode:
    // - normalize members' name (shader.name ==> shader_name)
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)stageBytecode.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(stageBytecode, start);
            spv::Op opCode = asOpCode(stageBytecode, start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

            switch (opCode)
            {
                case spv::OpMemberName:
                {
                    //replace all '.' by '_'
                    replaceAllCharactersInliteralString(stageBytecode, start + 3, '.', '_');
                    break;
                }

                case spv::OpName:
                {
                    //replace all '.' by '_'
                    replaceAllCharactersInliteralString(stageBytecode, start + 2, '.', '_');
                    break;
                }

                case spv::OpFunction:
                case spv::OpTypeFunction:
                {
                    //all done at this point: can stop here
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }
    }

    return true;
}

bool SpxCompiler::SpxCompiler::InitDefaultHeader()
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

bool SpxCompiler::BuildConstsHashmap(unordered_map<uint32_t, pairIdPos>& mapHashPos)
{
    mapHashPos.clear();

    //We build the hashmap table for all types and consts
    //except for OpTypeXlslShaderClass types: (this type is only informational, never used as a type or result)
    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        spv::Id id = spvUndefinedId;
        if (isConstOp(opCode))
        {
            id = asId(start + 2);
        }
        else if (opCode == spv::OpFunction)
        {
            break; //no more consts after this point
        }

        if (id != spvUndefinedId)
        {
            uint32_t hashval = hashType(start);

#ifdef XKSLANG_DEBUG_MODE
            if (hashval == spirvbin_t::unused) return error("Failed to get the hashval for a const. constId: " + to_string(id));
#endif

            //we don't mind if we have several candidates with the same hashType (several identical consts)
            mapHashPos[hashval] = pairIdPos(id, start);
        }

        start += wordCount;
    }

    return true;
}

bool SpxCompiler::BuildTypesAndConstsHashmap(unordered_map<uint32_t, pairIdPos>& mapHashPos)
{
    mapHashPos.clear();

    //We build the hashmap table for all types and consts
    //except for OpTypeXlslShaderClass types: (this type is only informational, never used as a type or result)

#ifdef BUILD_BY_PARSING_THE_BYTECODE
    //Build the hasmmap by analysing the bytecode
    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        spv::Id id = spvUndefinedId;
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
            else if (opCode == spv::OpFunction)
            {
                break; //no more type or consts after this point
            }
        }

        if (id != spvUndefinedId)
        {
            uint32_t hashval = hashType(start);

            //We don't check for type collision for now (can have some side-effect to merge same structures with different names while having the same layout)
///#ifdef XKSLANG_DEBUG_MODE
///            if (hashval == spirvbin_t::unused) error("Failed to get the hashval for a const or type. Id: " + to_string(id));
///            if (mapHashPos.find(hashval) != mapHashPos.end())
///            {
///                // Warning: might cause some conflicts sometimes?
///                //return error(string("2 types have the same hashmap value. Ids: ") + to_string(mapHashPos[hashval].first) + string(", ") + to_string(id));
///                id = spvUndefinedId;  //by precaution we invalidate the id: we cannot choose between them
///                //hashval = hashType(start);
///            }
///#endif

            mapHashPos[hashval] = pairIdPos(id, start);
        }

        start += wordCount;
    }
#else
    //Build the hasmmap by analysing our list of pre-built objects
    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj != nullptr && (obj->GetKind() == ObjectInstructionTypeEnum::Const || obj->GetKind() == ObjectInstructionTypeEnum::Type))
        {
            const spv::Id id = obj->GetId();
            const unsigned int start = obj->GetBytecodeStartPosition();
            uint32_t hashval = hashType(start);

            //We don't check for type collision for now (can have some side-effect to merge same structures with different names while having the same layout)
///#ifdef XKSLANG_DEBUG_MODE
            ///            if (hashval == spirvbin_t::unused) error("Failed to get the hashval for a const or type. Id: " + to_string(id));
            ///            if (mapHashPos.find(hashval) != mapHashPos.end())
            ///            {
            ///                // Warning: might cause some conflicts sometimes?
            ///                //return error(string("2 types have the same hashmap value. Ids: ") + to_string(mapHashPos[hashval].first) + string(", ") + to_string(id));
            ///                id = spvUndefinedId;  //by precaution we invalidate the id: we cannot choose between them
            ///                //hashval = hashType(start);
            ///            }
///#endif
            mapHashPos[hashval] = pairIdPos(id, start);
        }
    }
#endif

    return true;
}

bool SpxCompiler::GetListAllMethodsInfo(std::vector<MethodInfo>& vecMethodsInfo)
{
    vecMethodsInfo.clear();

    for (auto it = vecAllFunctions.begin(); it != vecAllFunctions.end(); ++it)
    {
        FunctionInstruction* aFunction = *it;
        vecMethodsInfo.push_back(MethodInfo(
            aFunction->name,
            aFunction->shaderOwner == nullptr? "": aFunction->shaderOwner->shaderOriginalBaseName,
            aFunction->IsStage()
        ));
    }

    return true;
}

//Update the start and end position for all objects
bool SpxCompiler::UpdateAllObjectsPositionInTheBytecode()
{
    //Parse the list of all object data
    vector<ParsedObjectData> listParsedObjectsData;
    bool res = BuildDeclarationNameMapsAndObjectsDataList(listParsedObjectsData);
    if (!res) {
        return error("Failed to build maps");
    }

    uint32_t maxResultId = bound();
    unsigned int countParsedObjects = (unsigned int)listParsedObjectsData.size();
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

bool SpxCompiler::UpdateAllMaps()
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

    unsigned int countParsedObjects = (unsigned int)listParsedObjectsData.size();
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

bool SpxCompiler::BuildAllMaps()
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

    unsigned int countParsedObjects = (unsigned int)listParsedObjectsData.size();
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

bool SpxCompiler::DecorateObjects(vector<bool>& vectorIdsToDecorate)
{
    //======================================================================================================
    // Decorate objects attributes and relations

    //some decorate must be processed after we finish processing some others
    vector<int> vectorInstructionsToProcessAtTheEnd;

    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        switch (opCode)
        {
            case spv::OpDecorate:
            {
                const spv::Id typeId = asId(start + 1);
                if (typeId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[typeId]) break;

                spv::Decoration decoration = (spv::Decoration)asLiteralValue(start + 2);
                switch (decoration)
                {
                    case spv::Decoration::DecorationArrayStride:
                    {
                        TypeInstruction* type = GetTypeById(typeId);
                        if (type != nullptr)
                        {
                            if (!IsArrayType(type)) { error("An arrayStride decoration is applied to a non-array type: " + to_string(typeId)); break; }
                            int arrayStride = asLiteralValue(start + 3);
                            type->arrayStride = arrayStride;
                        }
                        break;
                    }

                    case spv::Decoration::DecorationBlock:
                    {
                        TypeInstruction* type = GetTypeById(typeId);
                        if (type == nullptr) { error("Cannot retrieve the type for which we parsed a block decorate instruction: " + to_string(typeId)); break; }
                        type->SetAsCBuffer();
                    }
                }

                break;
            }

            /*case spv::OpMemberProperties:
            {
                //if the member has a stream property, define the type as a stream struct
                const spv::Id typeId = asId(start + 1);
                if (typeId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[typeId]) break;

                break;
            }*/

            case spv::Op::OpCBufferProperties:
            {
                //will process this at the end, to make sure the types connection to their shader owner have been done
                const spv::Id typeId = asId(start + 1);
                if (typeId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[typeId]) break;
                vectorInstructionsToProcessAtTheEnd.push_back(start);
                break;
            }

            case spv::Op::OpBelongsToShader:
            {
                const spv::Id shaderId = asId(start + 1);
                const spv::Id objectId = asId(start + 2);

                if (objectId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[objectId]) break;

                ShaderClassData* shaderOwner = GetShaderById(shaderId);
                if (shaderOwner == nullptr) { error("undeclared shader owner for Id: " + to_string(shaderId)); break; }

                FunctionInstruction* function = GetFunctionById(objectId);
                if (function != nullptr) {
                    //a function is defined as being owned by a shader
#ifdef XKSLANG_DEBUG_MODE
                    if (function->GetShaderOwner() != nullptr) { error("The function already has a shader owner: " + function->GetMangledName()); break; }
                    if (shaderOwner->HasFunction(function))
                    {
                        error(string("The shader: ") + shaderOwner->GetName() + " already possesses the function: " + function->GetMangledName()); break;
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
                        TypeInstruction* typePointer = GetTypePointerPointingTo(type);
                        VariableInstruction* variable = GetVariablePointingTo(typePointer);

                        if (typePointer == nullptr) { error("cannot find the pointer type to the shader type: " + type->GetName()); break; }
                        if (variable == nullptr) { error("cannot find the variable for shader type: " + type->GetName()); break; }
#ifdef XKSLANG_DEBUG_MODE
                        if (type->GetShaderOwner() != nullptr) { error("The type already has a shader owner: " + type->GetName()); break; }
                        if (typePointer->GetShaderOwner() != nullptr) { error("The typePointer already has a shader owner: " + typePointer->GetName()); break; }
                        if (variable->GetShaderOwner() != nullptr) { error("The variable already has a shader owner: " + variable->GetName()); break; }
                        if (shaderOwner->HasType(type)) { error("The shader: " + shaderOwner->GetName() + " already possesses the type: " + type->GetName()); break; }
#endif
                        ShaderTypeData* shaderType = new ShaderTypeData(shaderOwner, type, typePointer, variable);
                        type->SetShaderOwner(shaderOwner);
                        typePointer->SetShaderOwner(shaderOwner);
                        variable->SetShaderOwner(shaderOwner);
                        shaderOwner->AddShaderType(shaderType);
                    }
                    else
                    {
                        error("unprocessed OpBelongsToShader instruction, invalid objectId: " + to_string(objectId));
                    }
                }
                break;
            }

            case spv::OpShaderInheritance:
            {
                //A shader inherits from some shader parents
                const spv::Id shaderId = asId(start + 1);

                if (shaderId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[shaderId]) break;

                ShaderClassData* shader = GetShaderById(shaderId);
                if (shader == nullptr) { error("undeclared shader for Id: " + to_string(shaderId)); break; }

                int countParents = wordCount - 2;
                for (int p = 0; p < countParents; ++p)
                {
                    const spv::Id parentShaderId = asId(start + 2 + p);
                    ShaderClassData* shaderParent = GetShaderById(parentShaderId);
                    if (shaderParent == nullptr) { error("undeclared parent shader for Id: " + to_string(parentShaderId)); break; }

//#ifdef XKSLANG_DEBUG_MODE
                    //A shader can inherit several time of the same shader, we just add it once
                    //if (shader->HasParent(shaderParent)) { error(string("shader: ") + shader->GetName() + string(" already inherits from parent: ") + shaderParent->GetName()); break; }
//#endif
                    if (!shader->HasParent(shaderParent))
                    {
                        shader->AddParent(shaderParent);
                    }
                }
                break;
            }

            case spv::OpShaderCompositionDeclaration:
            {
                const spv::Id shaderId = asId(start + 1);

                if (shaderId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[shaderId]) break;

                ShaderClassData* shaderCompositionOwner = GetShaderById(shaderId);
                if (shaderCompositionOwner == nullptr) { error("undeclared shader id: " + to_string(shaderId)); break; }

                int compositionId = asLiteralValue(start + 2);

                const spv::Id shaderCompositionTypeId = asId(start + 3);
                ShaderClassData* shaderCompositionType = GetShaderById(shaderCompositionTypeId);
                if (shaderCompositionType == nullptr) { error("undeclared shader type id: " + to_string(shaderCompositionTypeId)); break; }

                bool isStage = asLiteralValue(start + 4) == 0 ? false : true;
                bool isArray = asLiteralValue(start + 5) == 0? false: true;

                int count = asLiteralValue(start + 6);

                const string compositionVariableName = literalString(start + 7);

                ShaderCompositionDeclaration* shaderComposition = new ShaderCompositionDeclaration(compositionId, shaderCompositionOwner, shaderCompositionType, compositionVariableName,
                    isStage, isArray, count);
                this->AddNewShaderCompositionDeclaration(shaderCompositionOwner, shaderComposition);

                break;
            }

            //To be filled manually when we need it
            /*case spv::OpShaderInstancingPathItem:
            {
                const spv::Id shaderId = asId(start + 1);

                if (shaderId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[shaderId]) break;

                ShaderClassData* shader = GetShaderById(shaderId);
                if (shader == nullptr) { error("undeclared shader id: " + to_string(shaderId)); break; }

                int instanceLevel = asLiteralValue(start + 2);
                const spv::Id shaderCompositionOwnerId = asId(start + 3);
                int compositionNum = asLiteralValue(start + 4);
                int instanceNum = asLiteralValue(start + 5);

                if (shader->listInstancingPaths.size() != instanceLevel) { error("invalid instancing path level"); break; }
                shader->listInstancingPaths.push_back(ShaderInstancingPathItem(shaderId, instanceLevel, shaderCompositionOwnerId, compositionNum, instanceNum));
                
                break;
            }*/

            case spv::OpGSMethodProperties:
            {
                return error("OpGSMethodProperties: unprocessed yet");
            }

            case spv::Op::OpMethodProperties:
            {
                //a function is defined with some properties
                const spv::Id functionId = asId(start + 1);

                if (functionId >= vectorIdsToDecorate.size()) break;
                if (!vectorIdsToDecorate[functionId]) break;

                FunctionInstruction* function = GetFunctionById(functionId);
                if (function == nullptr) { error("undeclared function id: " + to_string(functionId)); break; }

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
                    case spv::XkslPropertyEnum::PropertyStage:
                        function->ParsedStageAttribute();
                        break;
                    }
                }
                break;
            }

            case spv::OpTypeFunction:
            case spv::OpFunction:
            {
                //no more decorations after this point
                start = end;
                break;
            }
        }

        start += wordCount;
    }

    for (unsigned int i = 0; i < vectorInstructionsToProcessAtTheEnd.size(); i++)
    {
        unsigned int start = vectorInstructionsToProcessAtTheEnd[i];
        spv::Op opCode = asOpCode(start);

        switch (opCode)
        {
            case spv::OpCBufferProperties:
            {
                const spv::Id typeId = asId(start + 1);
                spv::XkslPropertyEnum cbufferType = (spv::XkslPropertyEnum)asLiteralValue(start + 2);
                spv::XkslPropertyEnum cbufferStageEnum = (spv::XkslPropertyEnum)asLiteralValue(start + 3);

#ifdef XKSLANG_DEBUG_MODE
                if (cbufferType != spv::CBufferUndefined && cbufferType != spv::CBufferDefined) { error("Invalid cbuffer type property"); break; }
                if (cbufferStageEnum != spv::CBufferStage && cbufferStageEnum != spv::CBufferUnstage) { error("Invalid cbuffer stage property"); break; }
#endif

                int countMembers = asLiteralValue(start + 4);
                string cbufferSubpartName = literalString(start + 5);

                TypeInstruction* type = GetTypeById(typeId);
                if (type == nullptr) return error("Cannot find the type for Id: " + to_string(typeId));
                if (!type->IsCBuffer()) { error("The cbuffer type is not a cbuffer (missing block decorate?): " + type->GetName()); break; }
                if (countMembers <= 0) { error("Invalid number of members for the cbuffer: " + type->GetName()); break; }

                bool isStageCbuffer = (cbufferStageEnum == spv::CBufferStage ? true : false);
                bool isDefinedCbuffer = (cbufferType == spv::CBufferDefined ? true : false);

                CBufferTypeData* cbufferData = new CBufferTypeData(type->shaderOwner, typeId, type->GetName(), cbufferSubpartName, isDefinedCbuffer, isStageCbuffer, countMembers);
                type->SetCBufferData(cbufferData);

                break;
            }
        }
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

SpxCompiler::ObjectInstructionBase* SpxCompiler::CreateAndAddNewObjectFor(ParsedObjectData& parsedData)
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

            //The shader name and its original type name are set with the OpTypeXlslShaderClass instruction, we get them and update the shader
            spv::Op opCode = asOpCode(shader->bytecodeStartPosition);
#ifdef XKSLANG_DEBUG_MODE
            if (opCode != spv::OpTypeXlslShaderClass) { error("Inconsistent opcode"); return nullptr; }
#endif
            spv::XkslPropertyEnum shaderType = (spv::XkslPropertyEnum)asLiteralValue(shader->bytecodeStartPosition + 2);
            int countGenerics = asLiteralValue(shader->bytecodeStartPosition + 3);
            string shaderFullName = literalString(shader->bytecodeStartPosition + 4);
            int shaderNameCountWords = literalStringWords(shaderFullName);
            string shaderOriginalBaseName = literalString(shader->bytecodeStartPosition + 4 + shaderNameCountWords);

#ifdef XKSLANG_DEBUG_MODE
            if (shaderFullName != declarationName) { error("Shader name and declaration name are inconsistent"); return nullptr; }
#endif
            string errorMsg;
            if (!shader->SetShaderName(shaderOriginalBaseName, shaderFullName, countGenerics, errorMsg)) {
                error(errorMsg);
                error("Failed to set the shader name: " + shaderFullName);
                return nullptr;
            }

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

bool SpxCompiler::ComputeShadersLevel()
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

bool SpxCompiler::BuildDeclarationNameMapsAndObjectsDataList(vector<ParsedObjectData>& listParsedObjectsData)
{
    spv::Parameterize(); //first time only: we need to make sure the table is built before we can use it

    mapDeclarationName.clear();
    idPosR.clear();
    listParsedObjectsData.clear();

    int fnStart = 0;
    spv::Id fnResId = spv::NoResult;
    spv::Id fnTypeId = spv::NoResult;
    spv::Op fnOpCode;

    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        unsigned int instructionEnd = start + wordCount;

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

        //if (opCode == spv::Op::OpName)
        //{
        //    const spv::Id target = asId(start + 1);
        //    if (mapDeclarationName.find(target) == mapDeclarationName.end())
        //    {
        //        const string name = literalString(start + 2);
        //        mapDeclarationName[target] = name;
        //    }
        //}
        //else
        if (opCode == spv::Op::OpDeclarationName)
        {
            const spv::Id target = asId(start + 1);
            const string  name = literalString(start + 2);
            mapDeclarationName[target] = name;
        }
        else if (isConstOp(opCode))
        {
            listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Const, opCode, resultId, typeId, start, instructionEnd));
        }
        else if (isTypeOp(opCode))
        {
            if (opCode == spv::OpTypeXlslShaderClass)
            {
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Shader, opCode, resultId, typeId, start, instructionEnd));
            }
            else
            {
                ParsedObjectData data = ParsedObjectData(ObjectInstructionTypeEnum::Type, opCode, resultId, typeId, start, instructionEnd);
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
            listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Variable, opCode, resultId, typeId, start, instructionEnd));
        }
        else if (opCode == spv::Op::OpFunction)
        {
            if (fnStart != 0) { error("nested function found"); break; }
            fnStart = start;
            fnTypeId = typeId;
            fnResId = resultId;
            fnOpCode = opCode;
        }
        else if (opCode == spv::Op::OpFunctionEnd)
        {
            if (fnStart == 0) { error("function end without function start"); break; }
            if (fnResId == spv::NoResult) { error("function has no result iD"); break; }
            listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Function, fnOpCode, fnResId, fnTypeId, fnStart, instructionEnd));
            fnStart = 0;
        }
        else if (opCode == spv::Op::OpExtInstImport)
        {
            listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::HeaderProperty, opCode, resultId, typeId, start, instructionEnd));
        }

        start += wordCount;
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

//===========================================================================================================================//
//===========================================================================================================================//
string SpxCompiler::GetDeclarationNameForId(spv::Id id)
{
    auto it = mapDeclarationName.find(id);
    if (it == mapDeclarationName.end())
    {
        error(string("Id: ") + to_string(id) + string(" has no declaration name"));
        return string("");
    }
    return it->second;
}

bool SpxCompiler::GetDeclarationNameForId(spv::Id id, string& name)
{
    auto it = mapDeclarationName.find(id);
    if (it == mapDeclarationName.end())
        return false;

    name = it->second;
    return true;
}

SpxCompiler::HeaderPropertyInstruction* SpxCompiler::GetHeaderPropertyInstructionByOpCodeAndName(const spv::Op opCode, const string& name)
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

SpxCompiler::FunctionInstruction* SpxCompiler::GetTargetedFunctionByNameWithinShaderAndItsFamily(ShaderClassData* shader, const string& name)
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

int SpxCompiler::GetCountShaders()
{
    return (int)(vecAllShaders.size());
}

string SpxCompiler::GetShaderUniqueId()
{
    int uniqueId = (this->GetCountShaders() + 1);
    return "_id" + to_string(uniqueId);
}

SpxCompiler::ShaderClassData* SpxCompiler::GetShaderByName(const string& name)
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

bool SpxCompiler::GetStartPositionOfAllMemberDecorateInstructions(vector<unsigned int>& listStartPositionOfAllMemberDecorateInstructions)
{
    listStartPositionOfAllMemberDecorateInstructions.clear();

    unsigned int start = header_size;
    const unsigned int end = (unsigned int)spv.size();
    while (start < end)
    {
        unsigned int wordCount = asWordCount(start);
        spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
        if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

        switch (opCode)
        {
            case spv::OpMemberDecorate:
            {
                listStartPositionOfAllMemberDecorateInstructions.push_back(start);
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
    return true;
}

bool SpxCompiler::GetListAllFunctionCallInstructions(vector<FunctionCallInstructionData>& listFunctionCallInstructions)
{
    listFunctionCallInstructions.clear();
    for (auto itf = vecAllFunctions.begin(); itf != vecAllFunctions.end(); itf++)
    {
        FunctionInstruction* functionCalling = *itf;
        functionCalling->bytecodeStartPosition;

        unsigned int start = functionCalling->GetBytecodeStartPosition();
        const unsigned int end = functionCalling->GetBytecodeEndPosition();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

#ifdef XKSLANG_DEBUG_MODE
            if (wordCount == 0) return error("Corrupted bytecode: wordCount is equals to 0");
#endif

            switch (opCode)
            {
                case spv::OpFunctionCall:
                case spv::OpFunctionCallBaseResolved:
                case spv::OpFunctionCallBaseUnresolved:
                case spv::OpFunctionCallThroughStaticShaderClassCall:
                case spv::OpFunctionCallThroughCompositionVariable:
                {
                    spv::Id functionCalledId = asId(start + 3);
                    FunctionInstruction* functionCalled = GetFunctionById(functionCalledId);
                    if (functionCalled == nullptr) return error(string("Failed to retrieve the function for Id: ") + to_string(functionCalledId));

                    listFunctionCallInstructions.push_back(FunctionCallInstructionData(functionCalling, functionCalled, opCode, start));
                    
                    break;
                }
            }

            start += wordCount;
        }
    }

    return true;
}

SpxCompiler::ShaderClassData* SpxCompiler::GetShaderById(spv::Id id)
{
    if (id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Shader)
    {
        ShaderClassData* shader = dynamic_cast<ShaderClassData*>(obj);
        return shader;
    }
    return nullptr;
}

SpxCompiler::VariableInstruction* SpxCompiler::GetVariableByName(const string& name)
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

SpxCompiler::ObjectInstructionBase* SpxCompiler::GetObjectById(spv::Id id)
{
    if (id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];
    return obj;
}

SpxCompiler::FunctionInstruction* SpxCompiler::GetFunctionById(spv::Id id)
{
    if (id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Function)
    {
        FunctionInstruction* function = dynamic_cast<FunctionInstruction*>(obj);
        return function;
    }
    return nullptr;
}

SpxCompiler::ConstInstruction* SpxCompiler::GetConstById(spv::Id id)
{
    if (id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Const)
    {
        ConstInstruction* constInstr = dynamic_cast<ConstInstruction*>(obj);
        return constInstr;
    }
    return nullptr;
}

SpxCompiler::TypeInstruction* SpxCompiler::GetTypeById(spv::Id id)
{
    if (id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
    {
        TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
        return type;
    }
    return nullptr;
}

SpxCompiler::VariableInstruction* SpxCompiler::GetVariableById(spv::Id id)
{
    if (id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Variable)
    {
        VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
        return variable;
    }
    return nullptr;
}

SpxCompiler::TypeInstruction* SpxCompiler::GetTypePointerPointingTo(spv::StorageClass storageType, TypeInstruction* targetType)
{
    if (targetType == nullptr) return nullptr;

    TypeInstruction* res = nullptr;
    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
        {
            TypeInstruction* aType = dynamic_cast<TypeInstruction*>(obj);
            if (aType->GetTypePointed() == targetType)
            {
#ifdef XKSLANG_DEBUG_MODE
                if (aType->opCode != spv::OpTypePointer) error("A type pointing to another type must have an OpTypePointer opCode");
#endif

                spv::StorageClass pointerStorageClass = (spv::StorageClass)asLiteralValue(aType->GetBytecodeStartPosition() + 2);
                if (pointerStorageClass == storageType)
                {
                    if (res != nullptr) error("found 2 types pointing to the same type");
                    res = aType;
                }
            }
        }
    }
    return res;
}

SpxCompiler::TypeInstruction* SpxCompiler::GetTypePointerPointingTo(TypeInstruction* targetType)
{
    if (targetType == nullptr) return nullptr;

    TypeInstruction* res = nullptr;
    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
        {
            TypeInstruction* aType = dynamic_cast<TypeInstruction*>(obj);
            if (aType->GetTypePointed() == targetType)
            {
#ifdef XKSLANG_DEBUG_MODE
                if (aType->opCode != spv::OpTypePointer) error("A type pointing to another type must have an OpTypePointer opCode");
#endif

                if (res != nullptr) error("found 2 types pointing to the same type");
                res = aType;
            }
        }
    }
    return res;
}

SpxCompiler::VariableInstruction* SpxCompiler::GetVariablePointingTo(TypeInstruction* targetType)
{
    if (targetType == nullptr) return nullptr;

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

void SpxCompiler::stripBytecode(vector<range_t>& ranges)
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

bool SpxCompiler::remapAllIds(vector<uint32_t>& bytecode, unsigned int begin, unsigned int end, const vector<spv::Id>& remapTable)
{
    string errorMsg;
    if (!remapAllIds(bytecode, begin, end, remapTable, errorMsg))
    {
        error(errorMsg);
        return false;
    }
    return true;
}

bool SpxCompiler::remapAllIds(vector<uint32_t>& bytecode, unsigned int begin, unsigned int end, const vector<spv::Id>& remapTable, string& errorMsg)
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
bool SpxCompiler::remapAllInstructionIds(vector<uint32_t>& bytecode, unsigned int word, unsigned int& wordCount, const vector<spv::Id>& remapTable, string& errorMsg)
{
    const unsigned int instructionStart = word;
    wordCount = opWordCount(bytecode[instructionStart]);
    spv::Op opCode = opOpCode(bytecode[instructionStart]);
    const unsigned int nextInst = word++ + wordCount;

#ifdef XKSLANG_DEBUG_MODE
    if (nextInst > bytecode.size()) { errorMsg = "nextInst is out of bound"; return false; }
    if (wordCount == 0) { errorMsg = "wordcount equals 0"; return false; }
#endif

    // Base for computing number of operands; will be updated as more is learned
    unsigned int numOperands = wordCount - 1;

    // Read type and result ID from instruction desc table
    if (spv::InstructionDesc[opCode].hasType())
    {
#ifdef XKSLANG_DEBUG_MODE
        if (remapTable[bytecode[word]] == 0 || remapTable[bytecode[word]] == spvUndefinedId) {
            errorMsg = "Fail to remap the instructions IDs, no new value remapped for Id: " + to_string(bytecode[word]);
            return false;
        }
#endif
        bytecode[word] = remapTable[bytecode[word]];
        word++;
        --numOperands;
    }

    if (spv::InstructionDesc[opCode].hasResult())
    {
#ifdef XKSLANG_DEBUG_MODE
        if (remapTable[bytecode[word]] == 0 || remapTable[bytecode[word]] == spvUndefinedId) {
            errorMsg = "Fail to remap the instructions IDs, no new value remapped for Id: " + to_string(bytecode[word]);
            return false;
        }
#endif
        bytecode[word] = remapTable[bytecode[word]];
        word++;
        --numOperands;
    }

    // Extended instructions: currently, assume everything is an ID.
    // TODO: add whatever data we need for exceptions to that
    if (opCode == spv::OpExtInst)
    {
#ifdef XKSLANG_DEBUG_MODE
        if (remapTable[bytecode[word]] == 0 || remapTable[bytecode[word]] == spvUndefinedId) {
            errorMsg = "Fail to remap the instructions IDs, no new value remapped for Id: " + to_string(bytecode[word]);
            return false;
        }
#endif
        bytecode[word] = remapTable[bytecode[word]];

        word += 2; // instruction set, and instruction from set
        numOperands -= 2;

        for (unsigned op = 0; op < numOperands; ++op)
        {
#ifdef XKSLANG_DEBUG_MODE
            if (remapTable[bytecode[word]] == 0 || remapTable[bytecode[word]] == spvUndefinedId) {
                errorMsg = "Fail to remap the instructions IDs, no new value remapped for Id: " + to_string(bytecode[word]);
                return false;
            }
#endif
            bytecode[word] = remapTable[bytecode[word]];
            word++;
        }

        return true;
    }

    // Store IDs from instruction in our map
    for (int op = 0; numOperands > 0; ++op, --numOperands) {
        switch (spv::InstructionDesc[opCode].operands.getClass(op)) {
        case spv::OperandId:
        case spv::OperandScope:
        case spv::OperandMemorySemantics:
#ifdef XKSLANG_DEBUG_MODE
            if (remapTable[bytecode[word]] == 0 || remapTable[bytecode[word]] == spvUndefinedId){
                errorMsg = "Fail to remap the instructions IDs, no new value remapped for Id: " + to_string(bytecode[word]);
                return false;
            }
#endif
            bytecode[word] = remapTable[bytecode[word]];
            word++;
            break;

        case spv::OperandVariableIds:
            for (unsigned i = 0; i < numOperands; ++i)
            {
#ifdef XKSLANG_DEBUG_MODE
                if (remapTable[bytecode[word]] == 0 || remapTable[bytecode[word]] == spvUndefinedId) {
                    errorMsg = "Fail to remap the instructions IDs, no new value remapped for Id: " + to_string(bytecode[word]);
                    return false;
                }
#endif
                bytecode[word] = remapTable[bytecode[word]];
                word++;
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
        case spv::XkslShaderDataProperty:
        case spv::XkslSamplerStateFilter:
        case spv::XkslSamplerStateCompareFunction:
        case spv::XkslSamplerStateTextureAddressMode:
            ++word;
            break;

        case spv::XkslShaderDataProperties:
            return true;

        default:
            errorMsg = "Unhandled Operand Class";
            return false;
        }
    }

    return true;
}

bool SpxCompiler::flagAllIdsFromInstruction(unsigned int word, spv::Op& opCode, unsigned int& wordCount, std::vector<bool>& listIdsUsed)
{
    wordCount = opWordCount(spv[word]);
    opCode = opOpCode(spv[word]);
    const unsigned int nextInst = word + wordCount;
    word++;

#ifdef XKSLANG_DEBUG_MODE
    if (nextInst > spv.size()) return error("nextInst is out of bound");
    if (wordCount == 0) return error("wordcount equals 0");
#endif

    // Base for computing number of operands; will be updated as more is learned
    unsigned int numOperands = wordCount - 1;

    // Read type and result ID from instruction desc table
    if (spv::InstructionDesc[opCode].hasType()) {
        //listIds.push_back(asId(word++));
        listIdsUsed[asId(word++)] = true;
        --numOperands;
    }

    if (spv::InstructionDesc[opCode].hasResult()) {
        //listIds.push_back(asId(word++));
        listIdsUsed[asId(word++)] = true;
        --numOperands;
    }

    // Extended instructions: currently, assume everything is an ID.
    // TODO: add whatever data we need for exceptions to that
    if (opCode == spv::OpExtInst) {
        listIdsUsed[asId(word)] = true;  //Add ExtInstImport Id

        word += 2; // instruction set, and instruction from set
        numOperands -= 2;

        for (unsigned op = 0; op < numOperands; ++op)
            listIdsUsed[asId(word++)] = true;

        return true;
    }

    // Store IDs from instruction in our map
    for (int op = 0; numOperands > 0; ++op, --numOperands)
    {
        const spv::OperandParameters& operands = spv::InstructionDesc[opCode].operands;
#ifdef XKSLANG_DEBUG_MODE
        if (op >= operands.getNum()) {
            return ("op is out of bound");
            return false;
        }
#endif
        switch (operands.getClass(op))
        {
        case spv::OperandId:
        case spv::OperandScope:
        case spv::OperandMemorySemantics:
            listIdsUsed[asId(word++)] = true;
            break;

        case spv::OperandVariableIds:
            for (unsigned i = 0; i < numOperands; ++i)
                listIdsUsed[asId(word++)] = true;
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
                ///     listIds.push_back(spv[word++]);   // label
                /// }
                return ("Unplugged operand for now.. to check later");
                return false;
            }
            else {
                return ("invalid OperandVariableLiteralId instruction");
                return false;
            }

            return true;
        }

        case spv::OperandLiteralString: {
            const int stringWordCount = literalStringWords(literalString(spv, word));
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
            return "Unhandled Operand Class";
        }
    }

    return true;
}

// inspired by spirvbin_t::processInstruction. Allow us to store the instruction data and ids
bool SpxCompiler::parseInstruction(const vector<uint32_t>& bytecode, unsigned int word, spv::Op& opCode, unsigned int& wordCount, spv::Id& type, spv::Id& result, vector<spv::Id>& listIds, string& errorMsg)
{
    const unsigned int instructionStart = word;
    wordCount = opWordCount(bytecode[instructionStart]);
    opCode = opOpCode(bytecode[instructionStart]);
    const unsigned int nextInst = word++ + wordCount;

#ifdef XKSLANG_DEBUG_MODE
    if (nextInst > bytecode.size()) {errorMsg = "nextInst is out of bound"; return false;}
    if (wordCount == 0) { errorMsg = "wordcount equals 0"; return false; }
#endif

    // Base for computing number of operands; will be updated as more is learned
    unsigned int numOperands = wordCount - 1;

    // Read type and result ID from instruction desc table
    if (spv::InstructionDesc[opCode].hasType()) {
        //listIds.push_back(asId(word++));
        type = bytecode[word++];
        --numOperands;
    }
    else type = spvUndefinedId;

    if (spv::InstructionDesc[opCode].hasResult()) {
        //listIds.push_back(asId(word++));
        result = bytecode[word++];
        --numOperands;
    }
    else result = spvUndefinedId;

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
        spv::OperandClass operandClass = operands.getClass(op);
        switch (operandClass)
        {
            case spv::OperandId:
            case spv::OperandScope:
            case spv::OperandMemorySemantics:
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
            case spv::XkslShaderDataProperty:
            case spv::XkslSamplerStateFilter:
            case spv::XkslSamplerStateCompareFunction:
            case spv::XkslSamplerStateTextureAddressMode:
                ++word;
                break;

            case spv::XkslShaderDataProperties:
                return true;

            default:
                errorMsg = "Unhandled Operand Class";
                return false;
        }
    }

    return true;
}

// inspired by spirvbin_t::processInstruction. Allow us to store the instruction data and ids
bool SpxCompiler::parseInstruction(unsigned int word, spv::Op& opCode, unsigned int& wordCount, spv::Id& type, spv::Id& result, vector<spv::Id>& listIds)
{
    string errorMsg;
    if (!parseInstruction(spv, word, opCode, wordCount, type, result, listIds, errorMsg))
    {
        error(errorMsg);
        return false;
    }
    return true;
}

//=====================================================================================================================================
//=====================================================================================================================================
bool SpxCompiler::SetNewAtomicValueUpdate(BytecodeUpdateController& bytecodeUpdateController, unsigned int pos, uint32_t value)
{
    bytecodeUpdateController.listAtomicUpdates.push_back(BytecodeValueToReplace(pos, value));
    return true;
}

BytecodePortionToReplace* SpxCompiler::SetNewPortionToReplace(BytecodeUpdateController& bytecodeUpdateController, unsigned int pos)
{
    bytecodeUpdateController.listPortionsToUpdates.push_back(BytecodePortionToReplace(pos));
    return &(bytecodeUpdateController.listPortionsToUpdates.back());
}

BytecodePortionToRemove* SpxCompiler::AddPortionToRemove(BytecodeUpdateController& bytecodeUpdateController, unsigned int position, unsigned int count)
{
    if (count == 0) return nullptr;

    auto itListPos = bytecodeUpdateController.listSortedPortionsToRemove.begin();
    while (itListPos != bytecodeUpdateController.listSortedPortionsToRemove.end())
    {
        if (position == itListPos->position) return nullptr;
        if (position > itListPos->position) break; //we sort the list from higher to smaller position

        if (position + count > itListPos->position) return nullptr; //we're overlapping
        itListPos++;
    }

    itListPos = bytecodeUpdateController.listSortedPortionsToRemove.insert(itListPos, BytecodePortionToRemove(position, count));
    return &(*itListPos);
}

BytecodeChunk* SpxCompiler::GetOrCreateNewBytecodeChunckToInsert(BytecodeUpdateController& bytecodeUpdateController, unsigned int instructionPos, BytecodeChunkInsertionTypeEnum insertionType, unsigned int offset)
{
    return CreateNewBytecodeChunckToInsert(bytecodeUpdateController, instructionPos, insertionType, offset, true);
}

BytecodeChunk* SpxCompiler::CreateNewBytecodeChunckToInsert(BytecodeUpdateController& bytecodeUpdateController, unsigned int instructionPos, BytecodeChunkInsertionTypeEnum insertionType,
    unsigned int offset, bool returnExisintChunkInCaseOfConflict)
{
    if (instructionPos == 0 || instructionPos > spv.size())
    {
        error("Invalid instruction position for creating a new Bytecode Chunck");
        return nullptr;
    }

    //get the position where to insert the new bytecode chunck
    unsigned int instructionWordCount = instructionPos == spv.size()? 0: asWordCount(instructionPos);
    unsigned int insertionPos;
    switch (insertionType)
    {
        case BytecodeChunkInsertionTypeEnum::InsertBeforeInstruction:
            insertionPos = instructionPos;
            break;
        case BytecodeChunkInsertionTypeEnum::InsertWithinInstruction:
            if (offset > instructionWordCount) {
                error("Invalid offset to insert a bytecode chunck within the instruction");
                return nullptr;
            }
            insertionPos = instructionPos + offset;
            break;
        case BytecodeChunkInsertionTypeEnum::InsertAfterInstruction:
            insertionPos = instructionPos + instructionWordCount;
            break;
        default:
            error("Invalid insertionType enum");
            return nullptr;
    }

    //insert and sort the bytecode chunck
    std::list<BytecodeChunk>& listSortedChunks = bytecodeUpdateController.listSortedChunksToInsert;
    auto itListPos = listSortedChunks.begin();
    while (itListPos != listSortedChunks.end())
    {
        if (insertionPos > itListPos->insertionPos) break; //we sort the list from higher to smaller position
        else if (insertionPos == itListPos->insertionPos)
        {
            //In case of conflict, we look which instruction get inserted first
            if (instructionPos == itListPos->instructionPos)
            {
                //same instruction updated: we then look at which position within the instruction
                if (insertionType == itListPos->insertionType)
                {
                    if (returnExisintChunkInCaseOfConflict) return &(*itListPos);

                    error("conflict: We're adding 2 chuncks with the same insertion type and at same position");
                    return nullptr;
                }
                else
                {
                    if ((int)insertionType > (int)(itListPos->insertionType)) break;
                }
            }
            else
            {
                if (instructionPos > itListPos->instructionPos) break;
                //else //do nothing
            }
        }
        
        itListPos++;
    }

    itListPos = listSortedChunks.insert(itListPos, BytecodeChunk(instructionPos, insertionType, insertionPos));
    return &(*itListPos);
}

bool SpxCompiler::ApplyBytecodeUpdateController(BytecodeUpdateController& bytecodeUpdateController)
{
    unsigned int bytecodeOriginalSize = (unsigned int)spv.size();

    //first : update all values and portions
    for (auto itau = bytecodeUpdateController.listAtomicUpdates.begin(); itau != bytecodeUpdateController.listAtomicUpdates.end(); itau++)
    {
        const BytecodeValueToReplace& atomicValueUpdate = *itau;

#ifdef XKSLANG_DEBUG_MODE
        if (atomicValueUpdate.pos >= bytecodeOriginalSize) { error("pos is out of bound"); break; }
#endif

        spv[atomicValueUpdate.pos] = atomicValueUpdate.value;
    }
    for (auto itau = bytecodeUpdateController.listPortionsToUpdates.begin(); itau != bytecodeUpdateController.listPortionsToUpdates.end(); itau++)
    {
        const BytecodePortionToReplace& portionUpdate = *itau;

#ifdef XKSLANG_DEBUG_MODE
        if (portionUpdate.pos + portionUpdate.values.size() >= bytecodeOriginalSize) { error("portion to update is out of bound"); break; }
#endif

        auto itdest = spv.begin() + portionUpdate.pos;
        for (auto itp = portionUpdate.values.begin(); itp != portionUpdate.values.end(); itp++)
        {
            *itdest++ = *itp;
        }
    }

    //Insert all new chuncks in the bytecode, update positions accordingly of chuncks to remove
    for (auto itbc = bytecodeUpdateController.listSortedChunksToInsert.begin(); itbc != bytecodeUpdateController.listSortedChunksToInsert.end(); itbc++)
    {
        const BytecodeChunk& bytecodeChunck = *itbc;
        if (bytecodeChunck.bytecode.size() == 0) continue;

        int countBytesInserted = 0;

#ifdef XKSLANG_DEBUG_MODE
        if (bytecodeChunck.insertionPos > bytecodeOriginalSize) { error("bytecode chunck is out of bound"); break; }
#endif

        spv.insert(spv.begin() + bytecodeChunck.insertionPos, bytecodeChunck.bytecode.begin(), bytecodeChunck.bytecode.end());
        countBytesInserted = (unsigned int)bytecodeChunck.bytecode.size();

        if (countBytesInserted > 0)
        {
            //for all portions to remove coming AFTER the position we just inserted into, we update their position
            for (auto itpr = bytecodeUpdateController.listSortedPortionsToRemove.begin(); itpr != bytecodeUpdateController.listSortedPortionsToRemove.end(); itpr++)
            {
                BytecodePortionToRemove& portionToRemove = *itpr;

                if (portionToRemove.position >= bytecodeChunck.insertionPos)
                {
                    portionToRemove.position += countBytesInserted;
                }
                else
                {
                    if (portionToRemove.position + portionToRemove.count > bytecodeChunck.insertionPos){ error("A portion to remove overlap an insertion chunck"); }
                    break;
                }
            }
        }
    }

    //Remove all portions to remove
    for (auto itpr = bytecodeUpdateController.listSortedPortionsToRemove.begin(); itpr != bytecodeUpdateController.listSortedPortionsToRemove.end(); itpr++)
    {
        BytecodePortionToRemove& portionToRemove = *itpr;
        spv.erase(spv.begin() + portionToRemove.position, spv.begin() + (portionToRemove.position + portionToRemove.count));
    }

    if (errorMessages.size() > 0) return false;
    return true;
}
