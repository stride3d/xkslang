//
// Copyright (C) 

#ifndef XKSLANG_SPX_MIXEROUTPUTSTAGE_H__
#define XKSLANG_SPX_MIXEROUTPUTSTAGE_H__

#include <vector>
#include <string>

#include "define.h"
#include "SpxBytecode.h"

namespace xkslang
{

class XkslMixerOutputStage
{
public:
    ShadingStageEnum stage;
    std::string entryPointName;
    SpvBytecode resultingBytecode;

    XkslMixerOutputStage(ShadingStageEnum stage, std::string entryPointName) : stage(stage), entryPointName(entryPointName) {}
};

}  // namespace xkslang

#endif  // XKSLANG_SPX_MIXEROUTPUTSTAGE_H__
