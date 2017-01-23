//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_DEFINE_H
#define XKSLANG_XKSLPARSER_DEFINE_H

#include <string>
//#include "glslang/Public/ShaderLang.h"

#define XKSLANG_DEBUG_MODE

namespace xkslang
{
    enum class ShadingStage
    {
        Vertex,
        Pixel,
        TessControl,
        TessEvaluation,
        Geometry,
        Compute,
    };

    std::string GetStageLabel(ShadingStage stage);

} //namespace xkslang

#endif  // XKSLANG_XKSLPARSER_DEFINE_H
