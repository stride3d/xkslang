//
// Copyright (C)

#include <cassert>

#include "define.h"
#include "glslang/Public/ShaderLang.h"

using namespace std;
using namespace xkslang;

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