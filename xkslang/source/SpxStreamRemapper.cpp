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
    status = SpxRemapperStatusEnum::Undefined;
}

SpxStreamRemapper::~SpxStreamRemapper()
{
    ClearAllMaps();
}

bool SpxStreamRemapper::MixWithSpxBytecode(const SpxBytecode& bytecode)
{
    if (status != SpxRemapperStatusEnum::Undefined && status != SpxRemapperStatusEnum::MixinInProgress) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    if (spv.size() == 0)
    {
        //just copy the full code into the remapper
        if (!SetBytecode(bytecode)) return false;
    }
    else
    {
        //merge the new bytecode
        if (!MergeWithBytecode(bytecode)) return false;
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::MergeWithBytecode(const SpxBytecode& bytecode)
{
    int maxCountIds = bound();

    bool res = BuildXkslStreamShadersParsingData();

    if (!res){
        return error("Error building XKSL shaders data map");
    }

    SpxStreamRemapper bytecodeToMerge;
    if (!bytecodeToMerge.SetBytecode(bytecode)) return false;
    res = bytecodeToMerge.BuildXkslStreamShadersParsingData();

    if (!res) {
        return error(string("Error building XKSL shaders data map from bytecode:") + bytecode.GetName());
    }

    /// //Check the list of shaders from the bytecode we want to merge
    /// vector<spv::Id> listShaderToMerge;
    /// for (auto sh = bytecodeToMerge.declaredShaderAndTheirLevel.begin(); sh != bytecodeToMerge.declaredShaderAndTheirLevel.end(); sh++)
    /// {
    ///     spv::Id shaderId = sh->first;
    ///     const string& shaderName = bytecodeToMerge.GetDeclarationNameForId(shaderId);
    /// 
    ///     //if (currentData.declaredShaderAndTheirLevel.find(shaderId) == currentData.declaredShaderAndTheirLevel.end())
    ///     {
    ///         //a new shader to merge
    ///         listShaderToMerge.push_back(shaderId);
    ///     }
    /// }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::SetBytecode(const SpxBytecode& bytecode)
{
    const std::vector<uint32_t>& spx = bytecode.getBytecodeStream();
    spv.clear();
    spv.insert(spv.end(), spx.begin(), spx.end());
    return true;
}

//Mixin is finalized: no more updates will be brought to the mixin bytecode after
bool SpxStreamRemapper::FinalizeMixin()
{
    if (status != SpxRemapperStatusEnum::MixinInProgress) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinBeingFinalized;

    validate();  //validate the header
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    //Build the list of all overriding methods
    if (!BuildOverridenFunctionMap()) {
        return error("Processing overriding functions failed");
    }

    //Remap all overriden functions
    if (!RemapAllOverridenFunctions()) {
        return error("Remapping overriding functions failed");
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

    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode) {
                case spv::OpFunctionCallBase:
                {
                    setOpCode(start, spv::OpFunctionCall);
                    break;
                }
                case spv::OpTypeXlslShaderClass:
                {
                    stripInst(start); //remove OpTypeXlslShaderClass type
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
                        stripInst(start);
                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    return true;
}

bool SpxStreamRemapper::RemapAllOverridenFunctions()
{
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
                        spv::Id overridingFunctionId = gotOverrided->second.first;
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
    mapOverridenFunctions.clear();

    bool res = BuildXkslStreamShadersParsingData();

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
        FunctionData* function = itfn->second;
        if (!function->isOverride) continue;

        spv::Id overridingFunctionId = function->id;
        const string& overringFunctionName = function->mangledName;
        ShaderClassData* functionShaderOwner = function->owner;
        if (functionShaderOwner == nullptr){
            return error(string("Overriding function does not belong to a known shader class:") + overringFunctionName);
        }
        int overridingFunctionShaderLevel = functionShaderOwner->level;

        //Check all parents classes for functions with same mangles name
        vector<ShaderClassData*> listShadersToCheckForOverrideWithinParents;
        listShadersToCheckForOverrideWithinParents.push_back(function->owner);

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
                    const string& aParentFunctionName = aFunctionFromParent->mangledName;
                    if (overringFunctionName == aParentFunctionName)
                    {
                        //Yeah: got a function to be overriden !
                        pair<spv::Id, int> overridingMethod(overridingFunctionId, overridingFunctionShaderLevel);

                        auto gotRegistered = mapOverridenFunctions.find(aFunctionFromParent->id);
                        if (gotRegistered == mapOverridenFunctions.end())
                        {
                            mapOverridenFunctions[aFunctionFromParent->id] = overridingMethod;
                        }
                        else
                        {
                            //only replace the override indirection if the level is higher
                            int currentLevel = gotRegistered->second.second;
                            if (overridingFunctionShaderLevel > currentLevel)
                            {
                                mapOverridenFunctions[aFunctionFromParent->id] = overridingMethod;
                            }
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
    //Map all local maps
    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    BuildMapDeclarationName();
    
    //==========================================================================================
    //==========================================================================================
    // Search for the shader entry point
    std::vector<bool> isFunction(bound(), false);
    for (const auto fn : fnPos)
    {
        isFunction[fn.first] = true;
    }

    spv::Id entryPointFunctionId = spv::NoResult;
    string unmangledEntryPointFunctionName;
    for (const auto& name : mapDeclarationName)
    {
        if (isFunction[name.first])
        {
            string unmangledFunctionName = name.second.substr(0, name.second.find('('));
            if (unmangledFunctionName == entryPointName)
            {
                //We take the first valid function
                entryPointFunctionId = name.first;
                unmangledEntryPointFunctionName = unmangledFunctionName;
                break;
            }
        }
    }
    if (entryPointFunctionId == spv::NoResult)
    {
        return error(string("Entry point not found: ") + entryPointName);
    }
    //Check if the entry point function has been overriden
    {
        auto gotOverrided = mapOverridenFunctions.find(entryPointFunctionId);
        if (gotOverrided != mapOverridenFunctions.end())
        {
            entryPointFunctionId = gotOverrided->second.first;
        }
    }

    //==========================================================================================
    //==========================================================================================
    //save the current bytecode
    std::vector<uint32_t> bytecodeBackup;// = output.getWritableBytecodeStream();
    bytecodeBackup.insert(bytecodeBackup.end(), spv.begin(), spv.end());

    //==========================================================================================
    //==========================================================================================
    // Update the shader stage header
    if (!BuildAndSetShaderStageHeader(stage, entryPointFunctionId, unmangledEntryPointFunctionName))
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

bool SpxStreamRemapper::BuildAndSetShaderStageHeader(ShadingStage stage, spv::Id entryFunctionId, string unmangledFunctionName)
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

    if (entryFunctionId == spv::NoResult )
    {
        return error("Unknown entry function");
    }

    spv::ExecutionModel model = GetShadingStageExecutionMode(stage);
    if (model == spv::ExecutionModelMax)
    {
        return error("Unknown stage");
    }

    vector<unsigned int> stageHeader;

    //opEntryPoint: set the stage model, and entry point
    spv::Instruction entryPointInstr(spv::OpEntryPoint);
    entryPointInstr.addImmediateOperand(model);
    entryPointInstr.addIdOperand(entryFunctionId);
    entryPointInstr.addStringOperand(unmangledFunctionName.c_str());
    entryPointInstr.dump(stageHeader);

    dceEntryPoints(); //remove current entry points
    strip();

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

    mapFunctionsById.clear();
    mapShadersById.clear();
    mapShadersByName.clear();
    mapDeclarationName.clear();
}

bool SpxStreamRemapper::BuildXkslStreamShadersParsingData()
{
    cout << "BuildXkslStreamShadersParsingData!!" << endl;

    ClearAllMaps();

    buildLocalMaps();
    BuildMapDeclarationName();

    //create all shader data objects
    for (const auto typeStart : typeConstPos)
    {
        if (asOpCode(typeStart) == spv::OpTypeXlslShaderClass)
        {
            spv::Id shaderId = asTypeConstId(typeStart);
            string shaderName;
            if (!GetDeclarationNameForId(shaderId, shaderName)){
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

    //create all functions data objects
    for (auto fn = fnPos.begin(); fn != fnPos.end(); fn++)
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

    int processLineNb = 0;
    process(
        [&](spv::Op opCode, unsigned start) {
            processLineNb++;
            unsigned word = start + 1;
            spv::Id  typeId = spv::NoResult;

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
                        function->isOverride = true;
                        break;
                    }

                    case spv::DecorationBelongsToShader:
                    {
                        FunctionData* function = IsFunction(target);
                        if (function != nullptr) {
                            //a function is defined as being owned by a shader
                            const std::string ownerName = literalString(start + 3);
                            ShaderClassData* shaderOwner = GetShaderByName(ownerName);
                            if (shaderOwner == nullptr)
                                {error(string("undeclared parent shader:") + ownerName); break;}
                            if (function->owner != nullptr)
                                {error(string("Function already has a shader owner:") + function->mangledName); break;}

                            function->owner = shaderOwner;
                            shaderOwner->functionsList.push_back(function);
                        }
                        else
                        {
                            int lgksldj = 545;
                        }
                        break;
                    }

                    case spv::DecorationShaderInheritFromParent:
                    {
                        //A shader inherits from a parent
                        ShaderClassData* shader = GetShaderById(target);
                        if (shader == nullptr)
                            {error(string("undeclared shader id:") + to_string(target)); break;}

                        const std::string parentName = literalString(start + 3);
                        ShaderClassData* shaderParent = GetShaderByName(parentName);
                        if (shaderParent == nullptr)
                            {error(string("undeclared parent shader:") + parentName); break;}
                        
                        shader->parentsList.push_back(shaderParent);
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

void SpxStreamRemapper::BuildMapDeclarationName()
{
    mapDeclarationName.clear();

    //build maps needed for XKSL extensions
    //build declaration name maps
    process(
        [&](spv::Op opCode, unsigned start) {
            unsigned word = start + 1;
            spv::Id  typeId = spv::NoResult;

            if (opCode == spv::Op::OpDecorate) {
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
            return true;
        },
        spx_op_fn_nop
    );
}

//remove entry points operation
void SpxStreamRemapper::dceEntryPoints()
{
    process(
        [&](spv::Op opCode, unsigned start)
        {
            if (opCode == spv::OpEntryPoint)
                stripInst(start);
            return true;
        },
        spx_op_fn_nop
    );
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
    if (it == mapFunctionsById.end()) 
    {
        return nullptr;
    }
    return it->second;
}
