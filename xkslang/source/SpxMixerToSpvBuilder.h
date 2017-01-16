//
// Copyright (C) 

#ifndef XKSLANG_SPV_BUILDER_H__
#define XKSLANG_SPV_BUILDER_H__

#include <string>
#include <vector>

#include "SPIRV/spirv.hpp"

#include "define.h"
#include "SpxBytecode.h"
#include "XkslMixer.h"

namespace spv
{
    class Builder;
}

namespace xkslang
{

//=====================================================================================================================//
//=====================================================================================================================//

//=============================================================================================================//
//=============================================  SpvBuilder  =================================================//
//============================================================================================================//
class SpxMixerToSpvBuilder
{
public:
    SpxMixerToSpvBuilder();
    virtual ~SpxMixerToSpvBuilder();

    bool SetupBuilder(SPVFunction* entryFunction, ShadingStage stage);
    void FinalizeBuilder();

    bool DumpToSpvBytecode(SpvBytecode& bytecode);
    void DumpLoggerMessage(std::vector<std::string>& msgs);

private:

    spv::Builder* builder;
    spv::SpvBuildLogger* logger;
    spv::Id stdBuiltins;
    spv::Instruction* entryPoint;

private:
    spv::ExecutionModel TranslateShadingStageToExecutionModel(ShadingStage stage);
};

}  // namespace xkslang

#endif  // XKSLANG_SPV_BUILDER_H__
