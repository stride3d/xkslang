//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_DEFINE_H
#define XKSLANG_XKSLPARSER_DEFINE_H

#include <string>
//#include "glslang/Public/ShaderLang.h"

#define XKSLANG_DEBUG_MODE

namespace xkslang
{
    enum class ShadingStageEnum
    {
        Vertex,
        Pixel,
        TessControl,
        TessEvaluation,
        Geometry,
        Compute,
    };

    std::string GetShadingStageLabel(ShadingStageEnum stage);

} //namespace xkslang

#endif  // XKSLANG_XKSLPARSER_DEFINE_H
