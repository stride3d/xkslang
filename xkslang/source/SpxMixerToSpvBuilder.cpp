//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

#include "SPIRV/SpvBuilder.h"
//#include "SPIRV/GlslangToSpv.h"
//#include "SPIRV/disassemble.h"
//#include "SPIRV/doc.h"
//#include "SPIRV/SPVRemapper.h"

#include "SpxMixerToSpvBuilder.h"
//#include "XkslMixer.h"
//#include "SpxStreamParser.h"

using namespace std;
using namespace spv;
using namespace xkslang;

static const int GeneratorVersion = 1;
static const int LanguageVersion = 450;

//=============================================================================================================//
//=============================================================================================================//

SpxMixerToSpvBuilder::SpxMixerToSpvBuilder()
{
    logger  = nullptr;
    builder = nullptr;
}

SpxMixerToSpvBuilder::~SpxMixerToSpvBuilder()
{
    if (builder) delete builder;
    if (logger)  delete logger;
}

bool SpxMixerToSpvBuilder::DumpToSpvBytecode(SpvBytecode& bytecode)
{
    if (builder == nullptr) return false;

    builder->dump(bytecode.getWritableBytecodeStream());
    return true;
}

void SpxMixerToSpvBuilder::DumpLoggerMessage(vector<string>& msgs)
{
    if (logger == nullptr) return;
    logger->getAllMessages(msgs);
}

bool SpxMixerToSpvBuilder::SetupBuilder(SPVFunction* entryFunction, ShadingStage stage)
{
    logger  = new SpvBuildLogger();
    builder = new Builder((glslang::GetKhronosToolId() << 16) | GeneratorVersion, logger);

    builder->clearAccessChain();
    builder->setSource(spv::SourceLanguage::SourceLanguageUnknown, LanguageVersion);
    stdBuiltins = builder->import("GLSL.std.450");
    builder->setMemoryModel(spv::AddressingModelLogical, spv::MemoryModelGLSL450);

    string entryPointName = entryFunction == nullptr? "": entryFunction->declarationName;

    //add entrypoint details in the header
    Function* shaderEntry = builder->makeEntryPoint(entryPointName.c_str());
    entryPoint = builder->addEntryPoint(TranslateShadingStageToExecutionModel(stage), shaderEntry, entryPointName.c_str());

    return true;
}

//void TGlslangToSpvTraverser::finishSpv()
void SpxMixerToSpvBuilder::FinalizeBuilder()
{
    //TODO: remap entry function IO

    /*if (!entryPointTerminated) {
        builder.setBuildPoint(shaderEntry->getLastBlock());
        builder.leaveFunction();
    }

    // finish off the entry-point SPV instruction by adding the Input/Output <id>
    for (auto it = iOSet.cbegin(); it != iOSet.cend(); ++it)
        entryPoint->addIdOperand(*it);

    builder.eliminateDeadDecorations();*/
}

ExecutionModel SpxMixerToSpvBuilder::TranslateShadingStageToExecutionModel(ShadingStage stage)
{
    switch (stage) {
    case ShadingStage::Vertex:           return spv::ExecutionModelVertex;
    case ShadingStage::Pixel:            return spv::ExecutionModelFragment;
    case ShadingStage::TessControl:      return spv::ExecutionModelTessellationControl;
    case ShadingStage::TessEvaluation:   return spv::ExecutionModelTessellationEvaluation;
    case ShadingStage::Geometry:         return spv::ExecutionModelGeometry;
    case ShadingStage::Compute:          return spv::ExecutionModelGLCompute;
    default:
        assert(0);
        return spv::ExecutionModelFragment;
    }
}