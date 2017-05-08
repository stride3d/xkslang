//
// Copyright (C)

#include "EffectReflection.h"

using namespace std;
using namespace xkslang;

void EffectReflection::Clear()
{
    ConstantBuffers.clear();
}

string EffectReflection::GetEffectParameterReflectionClassLabel(EffectParameterReflectionClass parameterClass)
{
    switch (parameterClass)
    {
        case xkslang::EffectParameterReflectionClass::Undefined:           return "Undefined";
        case xkslang::EffectParameterReflectionClass::Scalar:              return "Scalar";
        case xkslang::EffectParameterReflectionClass::Vector:              return "Vector";
        case xkslang::EffectParameterReflectionClass::MatrixRows:          return "MatrixRows";
        case xkslang::EffectParameterReflectionClass::MatrixColumns:       return "MatrixColumns";
        case xkslang::EffectParameterReflectionClass::Object:              return "Object";
        case xkslang::EffectParameterReflectionClass::Struct:              return "Struct";
        case xkslang::EffectParameterReflectionClass::InterfaceClass:      return "InterfaceClass";
        case xkslang::EffectParameterReflectionClass::InterfacePointer:    return "InterfacePointer";
        case xkslang::EffectParameterReflectionClass::Sampler:             return "Sampler";
        case xkslang::EffectParameterReflectionClass::ShaderResourceView:  return "ShaderResourceView";
        case xkslang::EffectParameterReflectionClass::ConstantBuffer:      return "ConstantBuffer";
        case xkslang::EffectParameterReflectionClass::TextureBuffer:       return "TextureBuffer";
        case xkslang::EffectParameterReflectionClass::UnorderedAccessView: return "UnorderedAccessView";
        case xkslang::EffectParameterReflectionClass::Color:               return "Color";
        default: return "Unknown";
    }
}

std::string EffectReflection::GetEffectParameterReflectionTypeLabel(EffectParameterReflectionType parameterClass)
{
    switch (parameterClass)
    {
        case xkslang::EffectParameterReflectionType::Undefined:                    return "Undefined";
        case xkslang::EffectParameterReflectionType::Void:                         return "Void";
        case xkslang::EffectParameterReflectionType::Bool:                         return "Bool";
        case xkslang::EffectParameterReflectionType::Int:                          return "Int";
        case xkslang::EffectParameterReflectionType::Float:                        return "Float";
        case xkslang::EffectParameterReflectionType::String:                       return "String";
        case xkslang::EffectParameterReflectionType::Texture:                      return "Texture";
        case xkslang::EffectParameterReflectionType::Texture1D:                    return "Texture1D";
        case xkslang::EffectParameterReflectionType::Texture2D:                    return "Texture2D";
        case xkslang::EffectParameterReflectionType::Texture3D:                    return "Texture3D";
        case xkslang::EffectParameterReflectionType::TextureCube:                  return "TextureCube";
        case xkslang::EffectParameterReflectionType::Sampler:                      return "Sampler";
        case xkslang::EffectParameterReflectionType::Sampler1D:                    return "Sampler1D";
        case xkslang::EffectParameterReflectionType::Sampler2D:                    return "Sampler2D";
        case xkslang::EffectParameterReflectionType::Sampler3D:                    return "Sampler3D";
        case xkslang::EffectParameterReflectionType::SamplerCube:                  return "SamplerCube";
        case xkslang::EffectParameterReflectionType::UInt:                         return "UInt";
        case xkslang::EffectParameterReflectionType::UInt8:                        return "UInt8";
        case xkslang::EffectParameterReflectionType::Buffer:                       return "Buffer";
        case xkslang::EffectParameterReflectionType::ConstantBuffer:               return "ConstantBuffer";
        case xkslang::EffectParameterReflectionType::TextureBuffer:                return "TextureBuffer";
        case xkslang::EffectParameterReflectionType::Texture1DArray:               return "Texture1DArray";
        case xkslang::EffectParameterReflectionType::Texture2DArray:               return "Texture2DArray";
        case xkslang::EffectParameterReflectionType::Texture2DMultisampled:        return "Texture2DMultisampled";
        case xkslang::EffectParameterReflectionType::Texture2DMultisampledArray:   return "Texture2DMultisampledArray";
        case xkslang::EffectParameterReflectionType::TextureCubeArray:             return "TextureCubeArray";
        case xkslang::EffectParameterReflectionType::Double:                       return "Double";
        case xkslang::EffectParameterReflectionType::RWTexture1D:                  return "RWTexture1D";
        case xkslang::EffectParameterReflectionType::RWTexture1DArray:             return "RWTexture1DArray";
        case xkslang::EffectParameterReflectionType::RWTexture2D:                  return "RWTexture2D";
        case xkslang::EffectParameterReflectionType::RWTexture2DArray:             return "RWTexture2DArray";
        case xkslang::EffectParameterReflectionType::RWTexture3D:                  return "RWTexture3D";
        case xkslang::EffectParameterReflectionType::RWBuffer:                     return "RWBuffer";
        case xkslang::EffectParameterReflectionType::ByteAddressBuffer:            return "ByteAddressBuffer";
        case xkslang::EffectParameterReflectionType::RWByteAddressBuffer:          return "RWByteAddressBuffer";
        case xkslang::EffectParameterReflectionType::StructuredBuffer:             return "StructuredBuffer";
        case xkslang::EffectParameterReflectionType::RWStructuredBuffer:           return "RWStructuredBuffer";
        case xkslang::EffectParameterReflectionType::AppendStructuredBuffer:       return "AppendStructuredBuffer";
        case xkslang::EffectParameterReflectionType::ConsumeStructuredBuffer:      return "ConsumeStructuredBuffer";
        default: return "Unknown";
    }
}