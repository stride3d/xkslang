//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_DEFINE_H
#define XKSLANG_XKSLPARSER_DEFINE_H

#include <string>
//#include "glslang/Public/ShaderLang.h"

///#define XKSLANG_DEBUG_MODE (now defined in the project properties in Debug mode)

//If true: we return an error when parsing a shader not complying with the new xksl rules
//#define XKSLANG_ENFORCE_NEW_XKSL_RULES

#define XKSLANG_ADD_NAMES_AND_DEBUG_DATA_INTO_BYTECODE

//By default SPIRV-Cross insert all cbuffer members into an intermediary struct
//This define allows a workaround which removes this intermediaty struct and insert the members directly within the cbuffer declaration
#define SPIRV_CROSS_INSERT_CBUFFER_MEMBERS_DIRECTLY_WITHIN_CBUFFER_DECLARATION

#define MIXIN_ADD_COMPOSITION_RETURN_AN_ERROR_IF_A_NON_ARRAYED_COMPOSITION_RECEIVES_MORE_THAN_1_INSTANCE

namespace xkslang
{
    enum class ShadingStageEnum : int32_t
    {
        Undefined = -1,
        Vertex = 0,
        Pixel = 1,
        TessControl = 2,
        TessEvaluation = 3,
        Geometry = 4,
        Compute = 5,
    };

    class MethodInfo
    {
    public:
        std::string Name;
        std::string ShaderClassName;
        bool IsStage;

        MethodInfo() {}
        MethodInfo(const std::string& name, const std::string& shaderClassName, bool isStage)
            : Name(name), ShaderClassName(shaderClassName), IsStage(isStage) {}
    };

    class ShaderCompositionInfo
    {
    public:
        std::string ShaderOwner;
        std::string CompositionShaderType;
        std::string CompositionVariableName;
        bool IsArray;
        int CompositionCountInstances;
        std::string OverridenBy;

        ShaderCompositionInfo() {}
        ShaderCompositionInfo(const std::string& shaderOwner, const std::string& shaderType, const std::string& variableName,
            bool isArray, int compositionCountInstances, const std::string& overridenBy)
            : ShaderOwner(shaderOwner), CompositionShaderType(shaderType), CompositionVariableName(variableName),
              IsArray(isArray), CompositionCountInstances(compositionCountInstances), OverridenBy(overridenBy) {}
    };

    bool IsAValidOutputStage(ShadingStageEnum stage);
    std::string GetShadingStageLabel(ShadingStageEnum stage);
    std::string GetShadingStageLabelShort(ShadingStageEnum stage);

    bool IsPow2(int pow2);
    int RoundToPow2(int number, int powerOf2);

} //namespace xkslang

#endif  // XKSLANG_XKSLPARSER_DEFINE_H
