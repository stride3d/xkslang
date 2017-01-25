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
static void copyStaticErrorMessagesTo(std::vector<std::string>& list)
{
    list.insert(list.end(), staticErrorMessages.begin(), staticErrorMessages.end());
}

void SpxStreamRemapper::copyMessagesTo(std::vector<std::string>& list)
{
    list.insert(list.end(), errorMessages.begin(), errorMessages.end());
}

void SpxStreamRemapper::error(const string& txt) const
{
    //we use a static vector first because we override a function defined as const, and parent class is calling this function >_<
    staticErrorMessages.push_back(txt);
}

bool SpxStreamRemapper::error(const std::string& txt)
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
    ClearAllMaps();
}

void SpxStreamRemapper::ClearAllMaps()
{
    if (mapShadersById.size() > 0)
    {
        for (auto it = mapShadersById.begin(); it != mapShadersById.end(); it++)
        {
            ShaderClassData* shader = it->second;
            delete shader;
        }
    }

    if (mapFunctionsById.size() > 0)
    {
        for (auto it = mapFunctionsById.begin(); it != mapFunctionsById.end(); it++)
        {
            FunctionData* func = it->second;
            delete func;
        }
    }

    if (mapTypeById.size() > 0)
    {
        for (auto it = mapTypeById.begin(); it != mapTypeById.end(); it++)
        {
            delete it->second;
        }
    }

    if (mapConstById.size() > 0)
    {
        for (auto it = mapConstById.begin(); it != mapConstById.end(); it++)
        {
            delete it->second;
        }
    }

    mapTypeById.clear();
    mapConstById.clear();
    mapFunctionsById.clear();
    mapShadersById.clear();
    mapShadersByName.clear();
    mapDeclarationName.clear();
}

bool SpxStreamRemapper::MixWithSpxBytecode(const SpxBytecode& bytecode)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    if (spv.size() == 0)
    {
        //just copy the full code into the remapper
        if (!SetBytecode(bytecode)) return false;


        //Build the list of all overriding methods
        if (!BuildOverridenFunctionMap())
        {
            return error("Processing overriding functions failed");
        }

        //Remap all overriden functions
        if (!RemapAllOverridenFunctions())
        {
            return error("Remapping overriding functions failed");
        }
    }
    else
    {
        //merge the new bytecode
        if (!MergeWithBytecode(bytecode)) return false;
    }

    if (errorMessages.size() > 0) return false;

    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}

bool SpxStreamRemapper::MergeWithBytecode(const SpxBytecode& bytecode)
{
    int maxCountIds = bound();

    bool res = BuildAllMaps();
    if (!res){
        return error("Error building XKSL shaders data map");
    }

    SpxStreamRemapper bytecodeToMerge;
    if (!bytecodeToMerge.SetBytecode(bytecode)) return false;
    res = bytecodeToMerge.BuildAllMaps();

    if (!res) {
        return error(string("Error building XKSL shaders data map from bytecode:") + bytecode.GetName());
    }

    //=============================================================================================================
    //=============================================================================================================
    //Get the list of shaders we want to merge (shaders not already in the destination bytecode)
    vector<ShaderClassData*> listShadersToMerge;
    for (auto itsh = bytecodeToMerge.mapShadersById.begin(); itsh != bytecodeToMerge.mapShadersById.end(); itsh++)
    {
        ShaderClassData* shaderToMerge = itsh->second;
        if (!HasShader(shaderToMerge->GetName()))
            listShadersToMerge.push_back(shaderToMerge);
    }

    if (errorMessages.size() > 0) return false;
    if (listShadersToMerge.size() == 0) return true;  //no new shaders to merge

    //=============================================================================================================
    //=============================================================================================================
    //Merge the selected shaders
    int newId = bound();

    spirvbin_t vecNamesToMerge;
    spirvbin_t vecDecoratesToMerge;
    spirvbin_t vecTypesConstsAndVariablesToMerge;

    vector<spv::Id> remapTable;
    remapTable.resize(bytecodeToMerge.bound(), unused);

    for (int is=0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is];

        //=============================================================================================================
        //add all types declared by the shader
        for (int t = 0; t < shaderToMerge->typesList.size(); ++t)
        {
            TypeData* typeToMerge = shaderToMerge->typesList[t];
            remapTable[typeToMerge->id] = newId++;

            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, typeToMerge->pos);
        }

        //=============================================================================================================
        //add all pointer to those types, plus the shader type definition
        for (auto it = bytecodeToMerge.mapTypeById.begin(); it != bytecodeToMerge.mapTypeById.end(); it++)
        {
            TypeData* type = it->second;
            switch (type->opCode)
            {
                case spv::OpTypePointer:
                {
                    unsigned start = type->pos;
                    const spv::Id resultId = bytecodeToMerge.asId(start + 1);
                    const spv::Id typeId = bytecodeToMerge.asId(start + 3);
                    if (remapTable[typeId] != unused)  //is it a pointer to a type to merge?
                    {
                        remapTable[resultId] = newId++;
                        bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, start);
                    }
                    break;
                }

                case spv::OpTypeXlslShaderClass:
                {
                    string name = bytecodeToMerge.GetDeclarationNameForId(type->id);
                    if (name == shaderToMerge->name)
                    {
                        unsigned start = type->pos;
                        const spv::Id resultId = bytecodeToMerge.asId(start + 1);
                        remapTable[resultId] = newId++;
                        bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, start);
                    }
                    break;
                }
            }
        }

        //=============================================================================================================
        //add all variables for those pointers
        bytecodeToMerge.process(
            [&](spv::Op opCode, unsigned start)
            {
                if (opCode == spv::OpVariable)
                {
                    const spv::Id resultId = bytecodeToMerge.asId(start + 2);
                    const spv::Id typeId = bytecodeToMerge.asId(start + 1);
                    if (remapTable[typeId] != unused)  //is it a variable to a type to merge?
                    {
                        remapTable[resultId] = newId++;
                        bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, start);
                    }
                }
                return true;
            },
            spx_op_fn_nop
        );

        //=============================================================================================================
        // get all name and decoration for the types / variables we need to merge
        bytecodeToMerge.process(
            [&](spv::Op opCode, unsigned start)
            {
                switch (opCode)
                {
                    case spv::OpName:
                    case spv::OpMemberName:
                    {
                        const spv::Id id = bytecodeToMerge.asId(start + 1);
                        if (remapTable[id] != unused)
                        {
                            bytecodeToMerge.CopyInstructionToVector(vecNamesToMerge.spv, start);
                        }
                        break;
                    }

                    case spv::OpDecorate:
                    case spv::OpMemberDecorate:
                    {
                        const spv::Id id = bytecodeToMerge.asId(start + 1);
                        if (remapTable[id] != unused)
                        {
                            bytecodeToMerge.CopyInstructionToVector(vecDecoratesToMerge.spv, start);
                        }
                        break;
                    }
                }
                return true;
            },
            spx_op_fn_nop
        );
    } //end shaderToMerge loop

    //=============================================================================================================
    //=============================================================================================================
    //Check for unmapped Ids among the list of types / variables to merge
    while (true)
    {
        vector<bool> unmappedIds;
        unmappedIds.resize(bytecodeToMerge.bound(), false);
        unsigned int countUnmappedIds = 0;

        unsigned pos = 0;
        unsigned end = vecTypesConstsAndVariablesToMerge.spv.size();
        spv::Op opCode;
        unsigned wordCount;
        std::vector<spv::Id> listIds;
        while (pos < end)
        {
            listIds.clear();
            if (!vecTypesConstsAndVariablesToMerge.parseInstruction(pos, opCode, wordCount, listIds))
                return error("Error parsing vecTypesConstsAndVariablesToMerge");

            const unsigned len = listIds.size();
            for (int i = 0; i < len; ++i)
            {
                const spv::Id id = listIds[i];
                if (remapTable[id] == unused)
                {
                    if (!unmappedIds[id])
                    {
                        unmappedIds[id] = true;
                        countUnmappedIds++;
                    }
                }
            }

            pos += wordCount;
        }

        if (countUnmappedIds == 0) break;

        //resolved the unmapped ids
        TypeData* typeToMerge;
        ConstData* constToMerge;
        for (unsigned i = 0; i < unmappedIds.size(); ++i)
        {
            if (!unmappedIds[i]) continue;
            spv::Id unmappedId = i;

            jhfdkgjhdskfhgk;
            if ((typeToMerge = bytecodeToMerge.IsType(unmappedId)) != nullptr)
            {
                //copy the full instructions: to optimize: we could check in the code to merge if the type declarataion already exists!
                remapTable[typeToMerge->id] = newId++;
                bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, typeToMerge->pos);
            }
            else if ((constToMerge = bytecodeToMerge.IsConst(unmappedId)) != nullptr)
            {
                //copy the full instructions: to optimize: we could check in the code to merge if the type declarataion already exists!
                remapTable[constToMerge->id] = newId++;
                bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, constToMerge->pos);
            }
            else
                return error(string("The unmapped Id is not a type, const or variable:") + to_string(unmappedId));
        }
    }

    //=============================================================================================================
    //=============================================================================================================
    //remap IDs for all types / variables / consts
    vecTypesConstsAndVariablesToMerge.processOnFullBytecode(
        spx_inst_fn_nop, // ignore instructions
        [&](spv::Id& id)
        {
            spv::Id newId = remapTable[id];
            if (newId == unused) error(string("Invalid remapper Id:") + to_string(id));
            else id = newId;
        }
    );

    //=============================================================================================================
    //=============================================================================================================
    //merge all types / variables / consts in the current bytecode
    bound(newId);

    //get the best positions where we can merge
    unsigned int firstDecoratePos = header_size;
    unsigned int firstFunctionPos = spv.size();
    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode)
            {
            case spv::OpDecorate:
            case spv::OpMemberDecorate:
                if (firstDecoratePos == header_size) firstDecoratePos = start;
                break;
            case spv::OpFunction:
                if (firstFunctionPos == header_size) firstFunctionPos = start;
                break;
            }
            return true;
        },
        spx_op_fn_nop
    );
    
    //=============================================================================================================
    //Merge types and variables
    {
        gfdksjgljldfg;
        vector<unsigned int>::iterator it = spv.begin() + firstFunctionPos;
        //spv.insert(it, vecTypesConstsAndVariablesToMerge.spv.begin(), vecTypesConstsAndVariablesToMerge.spv.end());
        spv.insert(spv.end(), vecTypesConstsAndVariablesToMerge.spv.begin(), vecTypesConstsAndVariablesToMerge.spv.end());
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::SetBytecode(const SpxBytecode& bytecode)
{
    const std::vector<uint32_t>& spx = bytecode.getBytecodeStream();
    spv.clear();
    spv.insert(spv.end(), spx.begin(), spx.end());

    validate();  //validate the header
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    return true;
}

//Mixin is finalized: no more updates will be brought to the mixin bytecode after
bool SpxStreamRemapper::FinalizeMixin()
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinBeingFinalized;

    validate();  //validate the header
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    //Convert SPIRX extensions to SPIRV
    if (!ConvertSpirxToSpirVBytecode()) {
        return error("Failed to convert SPIRX to SPIRV");
    }

    status = SpxRemapperStatusEnum::MixinFinalized;
    return true;
}

bool SpxStreamRemapper::ConvertSpirxToSpirVBytecode()
{
    //Convert OpIntructions
    // - OpFunctionCallBase --> OpFunctionCall (remapping of override functions has been completed)

    std::vector<range_t> vecStripRanges;
    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode) {
                case spv::OpFunctionCallBase:
                {
                    // change OpFunctionCallBase to OpFunctionCall
                    setOpCode(start, spv::OpFunctionCall);
                    break;
                }
                case spv::OpTypeXlslShaderClass:
                {
                    //remove OpTypeXlslShaderClass type
                    stripInst(start, vecStripRanges);
                    break;
                }
                case spv::OpName:
                case spv::OpDecorate:
                {
                    //id = asId(start + 1);
                    //if (id != spv::NoResult && isXkslShaderClassType[id] == 1)
                    //    stripInst(start);

                    //remove all XKSL decoration
                    const spv::Decoration dec = asDecoration(start + 2);
                    if (dec >= spv::DecorationDeclarationName && dec <= spv::DecorationMethodClone)
                        stripInst(start, vecStripRanges);
                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    stripBytecode(vecStripRanges);

    return true;
}

bool SpxStreamRemapper::RemapAllOverridenFunctions()
{
    std::unordered_map<spv::Id, FunctionData*> mapOverridenFunctions;
    for (auto itfn = mapFunctionsById.begin(); itfn != mapFunctionsById.end(); itfn++)
    {
        FunctionData* function = itfn->second;
        if (function->overridenBy != nullptr) mapOverridenFunctions[function->id] = function;
    }

    if (mapOverridenFunctions.size() == 0) return true; //nothing to override

    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode) {
                case spv::OpFunctionCall:
                {
                    spv::Id functionCalledId = asId(start + 3);

                    auto gotOverrided = mapOverridenFunctions.find(functionCalledId);
                    if (gotOverrided != mapOverridenFunctions.end())
                    {
                        spv::Id overridingFunctionId = gotOverrided->second->overridenBy->id;
                        spv[start + 3] = overridingFunctionId;
                    }

                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    return true;
}

bool SpxStreamRemapper::BuildOverridenFunctionMap()
{
    bool res = BuildAllMaps();
    if (!res)
    {
        return error("Error building XKSL shaders data map");
    }

    //========================================================================================================================//
    //========================================================================================================================//
    // check overrides for each functions declared with an override attribute
    // temporary implementation for now: simply override (replace) similar (same mangled name) functions from the function shader's parents classes
    for (auto itfn = mapFunctionsById.begin(); itfn != mapFunctionsById.end(); itfn++)
    {
        FunctionData* overridingFunction = itfn->second;
        if (!overridingFunction->hasOverride) continue;

        const string& overringFunctionName = overridingFunction->mangledName;
        ShaderClassData* functionShaderOwner = overridingFunction->owner;
        if (functionShaderOwner == nullptr){
            return error(string("Overriding function does not belong to a known shader class:") + overringFunctionName);
        }

        //Check all parents classes for functions with same mangles name
        vector<ShaderClassData*> listShadersToCheckForOverrideWithinParents;
        listShadersToCheckForOverrideWithinParents.push_back(functionShaderOwner);

        while (listShadersToCheckForOverrideWithinParents.size() > 0)
        {
            ShaderClassData* shaderToCheckForOverride = listShadersToCheckForOverrideWithinParents.back();
            listShadersToCheckForOverrideWithinParents.pop_back();

            //Check all parents
            for (int p=0; p<shaderToCheckForOverride->parentsList.size(); ++p)
            {
                ShaderClassData* parent = shaderToCheckForOverride->parentsList[p];
                listShadersToCheckForOverrideWithinParents.push_back(parent);

                //check all functions belonging to the parent shader
                for (int f = 0; f<parent->functionsList.size(); ++f)
                {
                    //compare the shader's functions name with the overring function name
                    FunctionData* aFunctionFromParent = parent->functionsList[f];
                    if (overringFunctionName == aFunctionFromParent->mangledName)
                    {
                        //Got a function to be overriden !
                        if (aFunctionFromParent->overridenBy == nullptr || aFunctionFromParent->overridenBy->owner->level <= overridingFunction->owner->level)
                        {
                            aFunctionFromParent->overridenBy = overridingFunction;
                        }
                    }
                }
            }
        }
    }

    if (errorMessages.size() > 0) return false;

    return true;
}

bool SpxStreamRemapper::GetMappedSpxBytecode(SpxBytecode& bytecode)
{
    if (spv.size() == 0)
    {
        return error("No code mapped");
    }

    std::vector<uint32_t>& bytecodeStream = bytecode.getWritableBytecodeStream();
    bytecodeStream.clear();
    bytecodeStream.insert(bytecodeStream.end(), spv.begin(), spv.end());

    return true;
}

bool SpxStreamRemapper::GenerateSpvStageBytecode(ShadingStage stage, std::string entryPointName, SpvBytecode& output)
{
    if (status != SpxRemapperStatusEnum::MixinFinalized)
    {
        return error("Invalid remapper status");
    }
    
    //==========================================================================================
    //==========================================================================================
    // Search for the shader entry point
    FunctionData* entryPointFunction = nullptr;
    for (auto it = mapFunctionsById.begin(); it != mapFunctionsById.end(); it++)
    {
        FunctionData* func = it->second;
        string unmangledFunctionName = func->mangledName.substr(0, func->mangledName.find('('));
        if (unmangledFunctionName == entryPointName)
        {
            entryPointFunction = func;
            //has the function been overriden?
            if (entryPointFunction->overridenBy != nullptr) entryPointFunction = entryPointFunction->overridenBy;
            break;
        }
    }
    if (entryPointFunction == nullptr)
    {
        return error(string("Entry point not found: ") + entryPointName);
    }

    //==========================================================================================
    //==========================================================================================
    //save the current bytecode
    std::vector<uint32_t> bytecodeBackup;// = output.getWritableBytecodeStream();
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
    //Clean and generate SPIRV bytecode

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
    std::vector<uint32_t>& outputSpv = output.getWritableBytecodeStream();
    outputSpv.clear();
    outputSpv.insert(outputSpv.end(), spv.begin(), spv.end());

    //reset the initial spx bytecode
    spv.clear();
    spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());

    return true;
}

bool SpxStreamRemapper::BuildAndSetShaderStageHeader(ShadingStage stage, FunctionData* entryFunction, string unmangledFunctionName)
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

    if (entryFunction == nullptr)
    {
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
    entryPointInstr.addIdOperand(entryFunction->id);
    entryPointInstr.addStringOperand(unmangledFunctionName.c_str());
    entryPointInstr.dump(stageHeader);

    //remove all current entry points
    {
        std::vector<range_t> vecStripRanges;
        process(
            [&](spv::Op opCode, unsigned start)
            {
                if (opCode == spv::OpEntryPoint)
                    stripInst(start, vecStripRanges);
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

spv::ExecutionModel SpxStreamRemapper::GetShadingStageExecutionMode(ShadingStage stage)
{
    switch (stage) {
    case ShadingStage::Vertex:           return spv::ExecutionModelVertex;
    case ShadingStage::Pixel:            return spv::ExecutionModelFragment;
    case ShadingStage::TessControl:      return spv::ExecutionModelTessellationControl;
    case ShadingStage::TessEvaluation:   return spv::ExecutionModelTessellationEvaluation;
    case ShadingStage::Geometry:         return spv::ExecutionModelGeometry;
    case ShadingStage::Compute:          return spv::ExecutionModelGLCompute;
    default:
        return spv::ExecutionModelMax;
    }
}

bool SpxStreamRemapper::BuildAllMaps()
{
    cout << "BuildAllMaps!!" << endl;

    ClearAllMaps();
    unordered_map<spv::Id, range_t> functionPos;

    bool res = BuildDeclarationName_ConstType_FunctionPos_Maps(functionPos);
    if (!res) {
        return error("Failed to build maps");
    }

    //======================================================================================================
    //create all shader data objects
    for (auto it = mapTypeById.begin(); it != mapTypeById.end(); it++)
    {
        TypeData* type = it->second;
        if (type->opCode == spv::OpTypeXlslShaderClass)
        {
            spv::Id shaderId = type->id;
            string shaderName;
            if (!GetDeclarationNameForId(shaderId, shaderName)) {
                error(string("Unknown name for shader type with Id:") + to_string(shaderId));
                break;
            }

            if (mapShadersById.find(shaderId) != mapShadersById.end()) {
                error("2 shaders have the same Id");
                break;
            }
            if (mapShadersByName.find(shaderName) != mapShadersByName.end()) {
                error("2 shaders have the same name");
                break;
            }

            ShaderClassData* shader = new ShaderClassData(shaderId, shaderName);
            mapShadersById[shaderId] = shader;
            mapShadersByName[shaderName] = shader;
        }
    }

    //======================================================================================================
    //create all functions data objects
    for (auto fn = functionPos.begin(); fn != functionPos.end(); fn++)
    {
        spv::Id functionId = fn->first;
        string functionName;
        if (!GetDeclarationNameForId(functionId, functionName)) {
            //some functions can be declared outside a shader class
            continue;
        }

        if (mapFunctionsById.find(functionId) != mapFunctionsById.end()) {
            error("2 functions have the same Id");
            break;
        }

        FunctionData* function = new FunctionData(functionId, functionName);
        mapFunctionsById[functionId] = function;
    }

    if (errorMessages.size() > 0) return false;

    //======================================================================================================
    // Process Decorate operations
    int processLineNb = 0;
    process(
        [&](spv::Op opCode, unsigned start) {
            processLineNb++;
            unsigned word = start + 1;

            if (opCode == spv::Op::OpDecorate) {
                const spv::Id target = asId(start + 1);
                const spv::Decoration dec = asDecoration(start + 2);

                switch (dec)
                {
                    case spv::DecorationMethodOverride:
                    {
                        //a function is defined with an override attribute
                        FunctionData* function = GetFunctionById(target);
                        if (function == nullptr)
                            {error(string("undeclared function id:") + to_string(target)); break;}
                        function->hasOverride = true;
                        break;
                    }

                    case spv::DecorationBelongsToShader:
                    {
                        FunctionData* function = IsFunction(target);
                        if (function != nullptr) {
                            //a function is defined as being owned by a shader
                            const std::string ownerName = literalString(start + 3);
                            ShaderClassData* shaderOwner = GetShaderByName(ownerName);

#ifdef XKSLANG_DEBUG_MODE
                            if (shaderOwner == nullptr) {error(string("undeclared parent shader:") + ownerName); break;}
                            if (function->owner != nullptr) {error(string("function already has a shader owner:") + function->mangledName); break;}
                            if (shaderOwner->HasFunction(function))
                            { error(string("shader:") + ownerName + string(" already possesses the function:") + function->mangledName); break; }
#endif
                            function->owner = shaderOwner;
                            shaderOwner->AddFunction(function);
                        }
                        else
                        {
                            TypeData* type = IsType(target);
                            if (type != nullptr)
                            {
                                const std::string ownerName = literalString(start + 3);
                                ShaderClassData* shaderOwner = GetShaderByName(ownerName);

#ifdef XKSLANG_DEBUG_MODE
                                if (shaderOwner == nullptr) { error(string("undeclared parent shader:") + ownerName); break; }
                                if (type->owner != nullptr) { error(string("type already has a shader owner:") + type->GetName()); break; }
                                if (shaderOwner->HasType(type)) { error(string("shader:") + ownerName + string(" already possesses the type:") + type->GetName()); break; }
#endif

                                type->owner = shaderOwner;
                                shaderOwner->AddType(type);
                            }
                            else
                            {
                                error("unprocessed DecorationBelongsToShader operand");
                            }
                        }
                        break;
                    }

                    case spv::DecorationShaderInheritFromParent:
                    {
                        //A shader inherits from a parent
                        ShaderClassData* shader = GetShaderById(target);
                        const std::string parentName = literalString(start + 3);
                        ShaderClassData* shaderParent = GetShaderByName(parentName);

#ifdef XKSLANG_DEBUG_MODE
                        if (shader == nullptr) {error(string("undeclared shader id:") + to_string(target)); break;}
                        if (shaderParent == nullptr) {error(string("undeclared parent shader:") + parentName); break;}
                        if (shader->HasParent(shaderParent)) {error(string("shader:") + shader->name + string(" already inherits from parent:") + parentName); break;}
#endif

                        shader->AddParent(shaderParent);
                        break;
                    }
                }

            }
            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) return false;

    //========================================================================================================================//
    // Set the shader levels (also detects cyclic shader inheritance)
    {
        bool allShaderSet = false;
        while (!allShaderSet)
        {
            allShaderSet = true;
            bool anyShaderUpdated = false;

            for (auto itsh = mapShadersById.begin(); itsh != mapShadersById.end(); itsh++)
            {
                spv::Id shaderId = itsh->first;
                ShaderClassData* shader = itsh->second;
                if (shader->level != -1) continue;  //shader already set

                if (shader->parentsList.size() == 0)
                {
                    shader->level = 0; //shader has no parent
                    anyShaderUpdated = true;
                    continue;
                }

                int maxParentLevel = -1;
                for (int p = 0; p<shader->parentsList.size(); ++p)
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
    }

    return true;
}

void SpxStreamRemapper::GetBytecodeTypeMap(std::unordered_map<spv::Id, unsigned>& mapTypes)
{
    mapTypes.clear();

    spv::Id fnResId = spv::NoResult;
    process(
        [&](spv::Op opCode, unsigned start) {
            /*if (isConstOp(opCode))
            {
                
                spv::Id id = asId(start + 2);
                if (mapConstById.find(id) != mapConstById.end()) {
                    error("2 consts have the same Id");
                }
                else
                {
                    ConstData* cd = new ConstData(opCode, id, start);
                    mapConstById[id] = cd;
                }
            }*/
            if (isTypeOp(opCode))
            {
                spv::Id id = asId(start + 1);
                mapTypes[id] = start;
            }
            return true;
        },
        spx_op_fn_nop
    );
}

bool SpxStreamRemapper::BuildDeclarationName_ConstType_FunctionPos_Maps(unordered_map<spv::Id, range_t>& functionPos)
{
    mapDeclarationName.clear();
    functionPos.clear();

    int fnStart = 0;
    spv::Id fnResId = spv::NoResult;
    process(
        [&](spv::Op opCode, unsigned start) {
            if (opCode == spv::Op::OpName)
            {
#ifdef XKSLANG_DEBUG_MODE
                const spv::Id target = asId(start + 1);
                const std::string name = literalString(start + 2);

                if (mapDeclarationName.find(target) == mapDeclarationName.end())
                    mapDeclarationName[target] = name;
#endif
            }
            else if (opCode == spv::Op::OpDecorate)
            {
                const spv::Id target = asId(start + 1);
                const spv::Decoration dec = asDecoration(start + 2);

                switch (dec)
                {
                    case spv::DecorationDeclarationName:
                    {
                        const std::string  name = literalString(start + 3);
                        mapDeclarationName[target] = name;
                        break;
                    }
                }
            }
            else if (isConstOp(opCode))
            {
                spv::Id id = asId(start + 2);
                if (mapConstById.find(id) != mapConstById.end()) {
                    error("2 consts have the same Id");
                }
                else
                {
                    ConstData* cd = new ConstData(opCode, id, start);
                    mapConstById[id] = cd;
                }
            }
            else if (isTypeOp(opCode))
            {
                spv::Id id = asId(start + 1);
                if (mapTypeById.find(id) != mapTypeById.end()) {
                    error("2 types have the same Id");
                }
                else
                {
                    TypeData* td = new TypeData(opCode, id, start);
#ifdef XKSLANG_DEBUG_MODE
                    string dataDebugName;
                    if (GetDeclarationNameForId(id, dataDebugName))
                        td->debugName = dataDebugName;
#endif //XKSLANG_DEBUG_MODE
                    mapTypeById[id] = td;
                }
            }
            else if (opCode == spv::Op::OpFunction)
            {
                if (fnStart != 0) error("nested function found");
                fnStart = start;
                fnResId = asId(start + 2);
            }
            else if (opCode == spv::Op::OpFunctionEnd)
            {
                if (fnStart == 0) error("function end without function start");
                if (fnResId == spv::NoResult) error("function has no result iD");
                functionPos[fnResId] = range_t(fnStart, start + asWordCount(start));
                fnStart = 0;
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
        error(string("Id:") + to_string(id) + string(" has no declaration name"));
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

SpxStreamRemapper::ShaderClassData* SpxStreamRemapper::GetShaderByName(const std::string& name)
{
    auto it = mapShadersByName.find(name);
    if (it == mapShadersByName.end())
    {
        error(string("Cannot find the shader:") + name);
        return nullptr;
    }
    return it->second;
}

SpxStreamRemapper::ShaderClassData* SpxStreamRemapper::HasShader(const std::string& name)
{
    auto it = mapShadersByName.find(name);
    if (it == mapShadersByName.end()) return false;
    return it->second;
}

SpxStreamRemapper::ShaderClassData* SpxStreamRemapper::GetShaderById(spv::Id id)
{
    auto it = mapShadersById.find(id);
    if (it == mapShadersById.end())
    {
        error(string("Cannot find the shader for Id:") + to_string(id));
        return nullptr;
    }
    return it->second;
}

SpxStreamRemapper::FunctionData* SpxStreamRemapper::GetFunctionById(spv::Id id)
{
    auto it = mapFunctionsById.find(id);
    if (it == mapFunctionsById.end())
    {
        error(string("Cannot find the function for Id:") + to_string(id));
        return nullptr;
    }
    return it->second;
}

SpxStreamRemapper::FunctionData* SpxStreamRemapper::IsFunction(spv::Id id)
{
    auto it = mapFunctionsById.find(id);
    if (it == mapFunctionsById.end()) return nullptr;
    return it->second;
}

SpxStreamRemapper::TypeData* SpxStreamRemapper::IsType(spv::Id id)
{
    auto it = mapTypeById.find(id);
    if (it == mapTypeById.end()) return nullptr;
    return it->second;
}

SpxStreamRemapper::ConstData* SpxStreamRemapper::IsConst(spv::Id id)
{
    auto it = mapConstById.find(id);
    if (it == mapConstById.end()) return nullptr;
    return it->second;
}

void SpxStreamRemapper::stripBytecode(vector<range_t>& ranges)
{
    if (ranges.empty()) // nothing to do
        return;

    // Sort strip ranges in order of traversal
    std::sort(ranges.begin(), ranges.end());

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