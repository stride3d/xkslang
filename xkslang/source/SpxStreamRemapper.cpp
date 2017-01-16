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
using namespace spv;
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

bool SpxStreamRemapper::MapSpxStream(const SpxBytecode& bytecode)
{
    staticErrorMessages.clear();
    this->errorMessages.clear();

    const std::vector<uint32_t>& spx = bytecode.getBytecodeStream();
    spv.clear();
    spv.insert(spv.end(), spx.begin(), spx.end());

    validate();
    if (staticErrorMessages.size() > 0){
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    buildLocalMaps();
    if (staticErrorMessages.size() > 0){
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    /*
    //Clean and generate a SPIRV bytecode
    dceXkslDecorateAndName();  //dce additionnal info added by Xksl extensions (name and decoration)
    dceFuncs(); //dce uncalled functions
    dceVars();  //dce unused function variables + decorations / name
    dceTypes(); //dce unused types

    strip();         //remove all strip bytecode
    stripDeadRefs(); //remove references to things we DCEed
    */

    status = RemapperStatusEnum::Valid;
    return true;
}

bool SpxStreamRemapper::GetMappedSpxBytecode(SpxBytecode& bytecode)
{
    if (GetStatus() != RemapperStatusEnum::Valid)
    {
        errorMessages.push_back("Invalid status");
        return false;
    }

    std::vector<uint32_t>& bytecodeStream = bytecode.getWritableBytecodeStream();
    bytecodeStream.clear();
    bytecodeStream.insert(bytecodeStream.end(), spv.begin(), spv.end());

    return true;
}

bool SpxStreamRemapper::GenerateSpvStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output)
{
    if (GetStatus() != RemapperStatusEnum::Valid)
    {
        errorMessages.push_back("Invalid status");
        return false;
    }

    //builder->clearAccessChain();
    //builder->setSource(spv::SourceLanguage::SourceLanguageUnknown, LanguageVersion);
    //stdBuiltins = builder->import("GLSL.std.450");
    //builder->setMemoryModel(spv::AddressingModelLogical, spv::MemoryModelGLSL450);

    //copy the current bytecode in the output
    std::vector<uint32_t>& outputSpv = output.getWritableBytecodeStream();
    outputSpv.clear();
    outputSpv.insert(outputSpv.end(), spv.begin(), spv.end());

    validate();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    //Clean and generate SPIRV bytecode
    dceXkslDecorateAndName();  //dce additionnal info added by Xksl extensions (name and decoration)
    dceFuncs(); //dce uncalled functions
    dceVars();  //dce unused function variables + decorations / name
    dceTypes(); //dce unused types

    strip();         //remove all strip bytecode
    stripDeadRefs(); //remove references to things we DCEed

    spv.swap(outputSpv);

    return true;
}

//Remove extra information added by SPRX extensions
// -Remove TypeXlslShaderClass's decorations and names
void SpxStreamRemapper::dceXkslDecorateAndName()
{
    msg(3, 2, std::string("DCE XkslDecorateAndName: "));

    std::vector<bool> isXkslType(bound(), false);
    for (const auto typeStart : typeConstPos)
    {
        if (asOpCode(typeStart) == spv::OpTypeXlslShaderClass)
            isXkslType[asTypeConstId(typeStart)] = true;
    }

    process(
        [&](spv::Op opCode, unsigned start)
        {
            spv::Id id = spv::NoResult;
            switch (opCode) {
                case spv::OpName:
                case spv::OpDecorate:
                //case spv::OpMemberName:
                //case spv::OpMemberDecorate:
                    id = asId(start + 1);
                    if (id != spv::NoResult && isXkslType[id] == 1)
                        stripInst(start);
                    break;
            }
            return true;
        },
        spx_op_fn_nop
    );
}