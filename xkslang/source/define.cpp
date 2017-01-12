//
// Copyright (C)

#include "define.h"

using namespace std;
using namespace xkslang;

string xkslang::GetStageLabel(ShadingStage stage)
{
    switch (stage)
    {
    case xkslang::ShadingStage::VertexStage:
        return "Vertex";
    case xkslang::ShadingStage::PixelStage:
        return "Pixel";
    }

    return "Undefined";
}
