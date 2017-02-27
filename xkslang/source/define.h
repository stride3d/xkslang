//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_DEFINE_H
#define XKSLANG_XKSLPARSER_DEFINE_H

#include <string>
//#include "glslang/Public/ShaderLang.h"

#define XKSLANG_DEBUG_MODE
#define XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE

namespace xkslang
{
    enum class ShadingStageEnum
    {
        Undefined = -1,
        Vertex = 0,
        Pixel = 1,
        TessControl = 2,
        TessEvaluation = 3,
        Geometry = 4,
        Compute = 5,
    };

    std::string GetShadingStageLabel(ShadingStageEnum stage);
    std::string GetShadingStageLabelShort(ShadingStageEnum stage);

} //namespace xkslang

#endif  // XKSLANG_XKSLPARSER_DEFINE_H
