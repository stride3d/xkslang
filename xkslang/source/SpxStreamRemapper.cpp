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

static const auto spx_inst_fn_nop = [](spv::Op, unsigned) { return false; };
static const auto spx_op_fn_nop = [](spv::Id&) {};
//===================================================================================================//

SpxStreamRemapper::SpxStreamRemapper(int verbose) : spirvbin_t(verbose)
{
    staticErrorMessages.clear(); //clear the list of error messages that we will receive from the parent class
    status = SpxRemapperStatusEnum::Undefined;
}

bool SpxStreamRemapper::MixSpxBytecodeStream(const SpxBytecode& bytecode)
{
    if (status != SpxRemapperStatusEnum::Undefined && status != SpxRemapperStatusEnum::MixinInProgress) {
        errorMessages.push_back("Invalid remappper status");
        return false;
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    const std::vector<uint32_t>& spx = bytecode.getBytecodeStream();

    if (spv.size() == 0)
    {
        //add the new code into the mixer
        spv.insert(spv.end(), spx.begin(), spx.end());
    }
    else
    {
        //add the codes minus the header
        errorMessages.push_back("Not implemented yet");
        return false;
    }

    return true;
}

//Mixin is finalized: no more updates will be brought to the mixin bytecode after
bool SpxStreamRemapper::FinalizeMixin()
{
    if (status != SpxRemapperStatusEnum::MixinInProgress) {
        errorMessages.push_back("Invalid remappper status");
        return false;
    }
    status = SpxRemapperStatusEnum::MixinBeingFinalized;

    validate();  //validate the header
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    //Build the list of all overriding methods
    if (!BuildOverridenFunctionMap()) {
        errorMessages.push_back("Processing overriding functions failed");
        return false;
    }

    //Remap all overriden functions
    if (!RemapAllOverridenFunctions()) {
        errorMessages.push_back("Remapping overriding functions failed");
        return false;
    }

    status = SpxRemapperStatusEnum::MixinFinalized;
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

    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    //========================================================================================================================//
    //========================================================================================================================//
    // build functions and classes attributes by parsing XKSL declaration attributes
    unordered_map<spv::Id, int> declaredShaderAndTheirLevel;        // list of xksl shader type declared in the bytecode, and their level
    unordered_map<spv::Id, bool> fnWithOverrideAttribute;           // methods having the override attributes
    unordered_map<spv::Id, spv::Id> fnOwnerShader;                  // method linked with the shader declaring it
    unordered_map<spv::Id, vector<spv::Id>> shaderParentsList;      // list of parents inherited by the shader
    unordered_map<spv::Id, vector<spv::Id>> shaderFunctionsList;    // list of functions declared within the shader

    for (const auto typeStart : typeConstPos)
    {
        if (asOpCode(typeStart) == spv::OpTypeXlslShaderClass)
        {
            spv::Id shaderId = asTypeConstId(typeStart);
            declaredShaderAndTheirLevel[shaderId] = -1;
        }
    }

    process(
        [&](spv::Op opCode, unsigned start) {
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
                        fnWithOverrideAttribute[target] = true;
                        break;
                    }
                    case spv::DecorationBelongsToShader:
                    {
                        //a function is defined as being owned by a shader
                        const std::string shaderName = literalString(start + 3);
                        spv::Id shaderId = spv::NoResult;
                        for (const auto& name : declarationNameMap) {
                            if ((declaredShaderAndTheirLevel.find(name.first) != declaredShaderAndTheirLevel.end()) && name.second == shaderName) {
                                shaderId = name.first;
                                break;
                            }
                        }
                        if (shaderId == spv::NoResult) {
                            errorMessages.push_back(string("undeclared shader:") + shaderName);
                            break;
                        }

                        fnOwnerShader[target] = shaderId;

                        //Add the function in the shader's functions list
                        unordered_map<spv::Id, vector<spv::Id>>::iterator got = shaderFunctionsList.find(shaderId);
                        if (got == shaderFunctionsList.end())
                        {
                            vector<spv::Id> vec;
                            vec.push_back(target);
                            shaderFunctionsList[shaderId] = vec;
                        }
                        else
                        {
                            got->second.push_back(target);
                        }

                        break;
                    }
                    case spv::DecorationShaderInheritFromParent:
                    {
                        //A shader inherits from a parent: get its parent id
                        const std::string parentName = literalString(start + 3);
                        spv::Id parentId = spv::NoResult;
                        for (const auto& name : declarationNameMap){
                            if ((declaredShaderAndTheirLevel.find(name.first) != declaredShaderAndTheirLevel.end()) && name.second == parentName){
                                parentId = name.first;
                                break;
                            }
                        }
                        if (parentId == spv::NoResult){
                            errorMessages.push_back(string("undeclared parent shader:") + parentName);
                            break;
                        }

                        unordered_map<spv::Id, vector<spv::Id>>::iterator got = shaderParentsList.find(target);
                        if (got == shaderParentsList.end())
                        {
                            vector<spv::Id> vec;
                            vec.push_back(parentId);
                            shaderParentsList[target] = vec;
                        }
                        else
                        {
                            got->second.push_back(parentId);
                        }
                        break;
                    }
                }

            }
            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) return false;

    if (fnWithOverrideAttribute.size() == 0) return true;  //no functions declared with override attributes, we can return here

    //========================================================================================================================//
    // Set the shader levels (this algorithm also detects cyclic shader inheritance)
    {
        bool allShaderSet = false;
        while (!allShaderSet)
        {
            allShaderSet = true;
            bool anyShaderUpdated = false;

            for (auto sh = declaredShaderAndTheirLevel.begin(); sh != declaredShaderAndTheirLevel.end(); sh++)
            {
                spv::Id shaderId = sh->first;
                if (sh->second != -1) continue;  //shader already set

                unordered_map<spv::Id, vector<spv::Id>>::iterator gotParents = shaderParentsList.find(shaderId);
                if (gotParents == shaderParentsList.end())
                {
                    sh->second = 0; //shader has no parent
                    anyShaderUpdated = true;
                    continue;
                }

                int maxParentLevel = -1;
                const vector<spv::Id>& vecParents = gotParents->second;
                for (int p = 0; p<vecParents.size(); ++p)
                {
                    spv::Id parentId = vecParents[p];
                    int parentLevel = declaredShaderAndTheirLevel[parentId];

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
                    sh->second = maxParentLevel + 1; //shader level
                    anyShaderUpdated = true;
                }
            }

            if (!anyShaderUpdated)
            {
                errorMessages.push_back("Cyclic inheritance detected among shaders");
                return false;
            }
        }
    }

    //========================================================================================================================//
    //========================================================================================================================//
    // check overrides for each functions declared with an override attribute
    // temporary implementation for now: simply override (replace) similar (same mangled name) functions from the function shader's parents classes
    for (auto fn = fnWithOverrideAttribute.begin(); fn != fnWithOverrideAttribute.end(); fn++)
    {
        spv::Id overridingFunctionId = fn->first;
        const string& overringFunctionName = declarationNameMap[overridingFunctionId];
        spv::Id overridingFunctionShaderId = fnOwnerShader[overridingFunctionId];
        if (overridingFunctionShaderId == 0 || declaredShaderAndTheirLevel.find(overridingFunctionShaderId) == declaredShaderAndTheirLevel.end()){
            errorMessages.push_back(string("Overriding function does not belong to a known shader class:") + overringFunctionName);
            return false;
        }
        int overridingFunctionShaderLevel = declaredShaderAndTheirLevel[overridingFunctionShaderId];

        vector<spv::Id> listShadersToCheckForOverrideWithinParents;
        listShadersToCheckForOverrideWithinParents.push_back(overridingFunctionShaderId);

        while (listShadersToCheckForOverrideWithinParents.size() > 0)
        {
            spv::Id classIdToCheckForOverride = listShadersToCheckForOverrideWithinParents.back();
            listShadersToCheckForOverrideWithinParents.pop_back();

            //Get list of parents
            unordered_map<spv::Id, vector<spv::Id>>::iterator gotParents = shaderParentsList.find(classIdToCheckForOverride);
            if (gotParents == shaderParentsList.end()) continue; //shader has no parent
            const vector<spv::Id>& vecParents = gotParents->second;

            //Check all parents
            for (int p=0; p<vecParents.size(); ++p)
            {
                spv::Id parentId = vecParents[p];
                listShadersToCheckForOverrideWithinParents.push_back(parentId);

                //check all functions belonging to the parent shader
                unordered_map<spv::Id, vector<spv::Id>>::iterator gotFunctions = shaderFunctionsList.find(parentId);
                if (gotFunctions == shaderFunctionsList.end()) continue; //the parent shader declares no functions
                const vector<spv::Id>& vecFunctions = gotFunctions->second;

                for (int f = 0; f<vecFunctions.size(); ++f)
                {
                    //compare the shader's functions name with the overring function name
                    spv::Id shaderFunctionId = vecFunctions[f];
                    const string& aParentFunctionName = declarationNameMap[shaderFunctionId];
                    if (overringFunctionName == aParentFunctionName)
                    {
                        //Yeah: got a function to be overriden !
                        pair<spv::Id, int> overridingMethod(overridingFunctionId, overridingFunctionShaderLevel);

                        auto gotRegistered = mapOverridenFunctions.find(shaderFunctionId);
                        if (gotRegistered == mapOverridenFunctions.end())
                        {
                            mapOverridenFunctions[shaderFunctionId] = overridingMethod;
                        }
                        else
                        {
                            //only replace the override indirection if the level is higher
                            int currentLevel = gotRegistered->second.second;
                            if (overridingFunctionShaderLevel > currentLevel)
                            {
                                mapOverridenFunctions[shaderFunctionId] = overridingMethod;
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
        errorMessages.push_back("No code mapped");
        return false;
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
        errorMessages.push_back("Invalid remappper status");
        return false;
    }

    //==========================================================================================
    //==========================================================================================
    //Map all local maps
    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

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
    for (const auto& name : declarationNameMap)
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
        errorMessages.push_back(string("Entry point not found: ") + entryPointName);
        return false;
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
        errorMessages.push_back("Error buiding the shader stage header");
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    //updating the header invalidated the maps
    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    //==========================================================================================
    //==========================================================================================
    //Clean and generate SPIRV bytecode

    dceXkslData();  //dce additionnal info added by Xksl extensions (name and decoration)
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
        errorMessages.push_back("Unknown entry function");
        return false;
    }

    spv::ExecutionModel model = GetShadingStageExecutionMode(stage);
    if (model == spv::ExecutionModelMax)
    {
        errorMessages.push_back("Unknown stage");
        return false;
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

void SpxStreamRemapper::buildLocalMaps()
{
    spirvbin_t::buildLocalMaps();

    declarationNameMap.clear();

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
                        declarationNameMap[target] = name;
                        break;
                    }
                    //case spv::DecorationShaderInheritFromParent:
                    //case spv::DecorationBelongsToShader:
                    //{
                    //    const std::string  name = literalString(start + 3);
                    //}
                    //case spv::DecorationMethodOverride:
                    //{
                    //    fnWithOverrideAttribute[target] = true;
                    //    break;
                    //}
                    //case spv::DecorationBelongsToShader:
                    //{
                    //    const std::string shaderName = literalString(start + 3);
                    //    fnOwnerClass[target] = shaderName;
                    //    break;
                    //}
                }

            }
            return true;
        },
        spx_op_fn_nop
    );
}

//Remove extra data added by SPRX extensions
void SpxStreamRemapper::dceXkslData()
{
    msg(3, 2, std::string("DCE dceXkslData: "));

    //std::vector<bool> isXkslShaderClassType(bound(), false);
    //for (const auto typeStart : typeConstPos)
    //{
    //    if (asOpCode(typeStart) == spv::OpTypeXlslShaderClass)
    //        isXkslShaderClassType[asTypeConstId(typeStart)] = true;
    //}

    process(
        [&](spv::Op opCode, unsigned start)
        {
            spv::Id id = spv::NoResult;
            switch (opCode) {
                case spv::OpTypeXlslShaderClass:
                {
                    stripInst(start); //remove OpTypeXlslShaderClass
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