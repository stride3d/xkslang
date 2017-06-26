//
// Copyright (C) 

#ifndef XKSLANG_SPX_MIXEROUTPUTSTAGE_H__
#define XKSLANG_SPX_MIXEROUTPUTSTAGE_H__

#include <vector>
#include <string>

#include "../Common/xkslangDefine.h"
#include "../Common/SpxBytecode.h"

namespace xkslang
{

class OutputStageBytecode
{
public:
    ShadingStageEnum stage;
    std::string entryPointName;
    SpvBytecode resultingBytecode;

    OutputStageBytecode(ShadingStageEnum stage, const std::string& entryPointName) : stage(stage), entryPointName(entryPointName) {}
};

}  // namespace xkslang

#endif  // XKSLANG_SPX_MIXEROUTPUTSTAGE_H__
