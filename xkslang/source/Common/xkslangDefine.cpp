//
// Copyright (C)

#include <cassert>

#include "xkslangDefine.h"
#include "glslang/Public/ShaderLang.h"

using namespace std;
using namespace xkslang;

bool xkslang::IsPow2(int pow2)
{
    if (pow2 <= 0) return false;
    return (pow2 & (pow2 - 1)) == 0;
}

int xkslang::RoundToPow2(int number, int powerOf2)
{
    if (!IsPow2(powerOf2)) return -1;
    int res = (number + powerOf2 - 1) & ~(powerOf2 - 1);
    return res;
}

bool xkslang::IsAValidOutputStage(ShadingStageEnum stage)
{
    switch (stage)
    {
        case ShadingStageEnum::Vertex:
        case ShadingStageEnum::Pixel:
        case ShadingStageEnum::TessControl:
        case ShadingStageEnum::TessEvaluation:
        case ShadingStageEnum::Geometry:
        case ShadingStageEnum::Compute:
            return true;
    }

    return false;
}

string xkslang::GetShadingStageLabel(ShadingStageEnum stage)
{
    switch (stage)
    {
        case ShadingStageEnum::Vertex: return "Vertex";
        case ShadingStageEnum::Pixel: return "Pixel";
        case ShadingStageEnum::TessControl: return "TessControl";
        case ShadingStageEnum::TessEvaluation: return "TessEvaluation";
        case ShadingStageEnum::Geometry: return "Geometry";
        case ShadingStageEnum::Compute: return "Compute";
    }
    return "Undefined";
}

string xkslang::GetShadingStageLabelShort(ShadingStageEnum stage)
{
    switch (stage)
    {
        case ShadingStageEnum::Vertex: return "VS";
        case ShadingStageEnum::Pixel: return "PS";
        case ShadingStageEnum::TessControl: return "TCS";
        case ShadingStageEnum::TessEvaluation: return "TES";
        case ShadingStageEnum::Geometry: return "GS";
        case ShadingStageEnum::Compute: return "CS";
    }
    return "Undef";
}

/*
EShLanguage xkslang::TranslateXkslStageToGlslangStage(ShadingStage stage)
{
    switch (stage) {
    case ShadingStage::Vertex:      return EShLangVertex;
    case ShadingStage::Pixel:       return EShLangFragment;
    case ShadingStage::TessControl: return EShLangTessControl;
    case ShadingStage::TessEval:    return EShLangTessEvaluation;
    case ShadingStage::Geometry:    return EShLangGeometry;
    case ShadingStage::Compute:     return EShLangCompute;
    }
    assert(0);
    return EShLangFragment;
}
*/