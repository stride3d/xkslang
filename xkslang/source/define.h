//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_DEFINE_H
#define XKSLANG_XKSLPARSER_DEFINE_H

#include <string>

namespace xkslang
{
    enum class ShadingStage
    {
        VertexStage,
        PixelStage,
    };

    std::string GetStageLabel(ShadingStage stage);

} //namespace xkslang

#endif  // XKSLANG_XKSLPARSER_DEFINE_H
