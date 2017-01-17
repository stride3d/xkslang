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

bool SpxStreamRemapper::MixSpxBytecodeStream(const SpxBytecode& bytecode)
{
    staticErrorMessages.clear();  //error messages receivable from parent class

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

    validate();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    //When a method is set as override: it will replace all methods defined with the same name
    if (!ProcessOverridingMethods())
    {
        errorMessages.push_back("Processing overriding methods failed");
        return false;
    }
    
    return true;
}

bool SpxStreamRemapper::ProcessOverridingMethods()
{
    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }



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
    if (spv.size() == 0)
    {
        errorMessages.push_back("No code mapped");
        return false;
    }

    //==========================================================================================
    //Check the bytecode header validity
    validate();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

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

    spv::Id functionId = spv::NoResult;
    string unmangledEntryPointFunctionName;
    for (const auto& name : declarationNameMap)
    {
        if (isFunction[name.first])
        {
            string unmangledFunctionName = name.second.substr(0, name.second.find('('));
            if (unmangledFunctionName == entryPointName)
            {
                if (functionId != spv::NoResult)
                {
                    errorMessages.push_back(string("2 or more entry points found: ") + entryPointName);
                    return false;
                }
                functionId = name.first;
                unmangledEntryPointFunctionName = unmangledFunctionName;
            }
        }
    }
    if (functionId == spv::NoResult)
    {
        errorMessages.push_back(string("Entry point not found: ") + entryPointName);
        return false;
    }

    //==========================================================================================
    //==========================================================================================
    //save the current bytecode
    std::vector<uint32_t> bytecodeBackup;// = output.getWritableBytecodeStream();
    bytecodeBackup.insert(bytecodeBackup.end(), spv.begin(), spv.end());

    //==========================================================================================
    //==========================================================================================
    // Update the shader stage header
    if (!BuildAndSetShaderStageHeader(stage, functionId, unmangledEntryPointFunctionName))
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
                if (dec == spv::DecorationDeclarationName)
                {
                    const std::string  name = literalString(start + 3);
                    declarationNameMap[target] = name;
                }
                //else if (dec == spv::DecorationShaderInheritFromParent || dec == spv::DecorationBelongsToShader)
                //{
                //    const std::string  name = literalString(start + 3);
                //    declarationNameMap[name] = target;
                //}

            }
            return true;
        },
        spx_op_fn_nop
    );
}

//remove current entry points
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