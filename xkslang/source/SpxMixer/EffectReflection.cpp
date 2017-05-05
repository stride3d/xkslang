//
// Copyright (C)

#include "EffectReflection.h"

using namespace std;
using namespace xkslang;

string EffectReflection::GetTypeReflectionClassLabel(TypeReflectionClass typeReflectionClass)
{
    switch (typeReflectionClass)
    {
        case xkslang::TypeReflectionClass::Undefined:           return "Undefined";
        case xkslang::TypeReflectionClass::Scalar:              return "Scalar";
        case xkslang::TypeReflectionClass::Vector:              return "Vector";
        case xkslang::TypeReflectionClass::MatrixRows:          return "MatrixRows";
        case xkslang::TypeReflectionClass::MatrixColumns:       return "MatrixColumns";
        case xkslang::TypeReflectionClass::Object:              return "Object";
        case xkslang::TypeReflectionClass::Struct:              return "Struct";
        case xkslang::TypeReflectionClass::InterfaceClass:      return "InterfaceClass";
        case xkslang::TypeReflectionClass::InterfacePointer:    return "InterfacePointer";
        case xkslang::TypeReflectionClass::Sampler:             return "Sampler";
        case xkslang::TypeReflectionClass::ShaderResourceView:  return "ShaderResourceView";
        case xkslang::TypeReflectionClass::ConstantBuffer:      return "ConstantBuffer";
        case xkslang::TypeReflectionClass::TextureBuffer:       return "TextureBuffer";
        case xkslang::TypeReflectionClass::UnorderedAccessView: return "UnorderedAccessView";
        case xkslang::TypeReflectionClass::Color:               return "Color";
        default: return "Unknown";
    }
}

std::string EffectReflection::GetTypeReflectionTypeLabel(TypeReflectionType typeReflectionType)
{
    switch (typeReflectionType)
    {
        case xkslang::TypeReflectionType::Undefined:                    return "Undefined";
        case xkslang::TypeReflectionType::Void:                         return "Void";
        case xkslang::TypeReflectionType::Bool:                         return "Bool";
        case xkslang::TypeReflectionType::Int:                          return "Int";
        case xkslang::TypeReflectionType::Float:                        return "Float";
        case xkslang::TypeReflectionType::String:                       return "String";
        case xkslang::TypeReflectionType::Texture:                      return "Texture";
        case xkslang::TypeReflectionType::Texture1D:                    return "Texture1D";
        case xkslang::TypeReflectionType::Texture2D:                    return "Texture2D";
        case xkslang::TypeReflectionType::Texture3D:                    return "Texture3D";
        case xkslang::TypeReflectionType::TextureCube:                  return "TextureCube";
        case xkslang::TypeReflectionType::Sampler:                      return "Sampler";
        case xkslang::TypeReflectionType::Sampler1D:                    return "Sampler1D";
        case xkslang::TypeReflectionType::Sampler2D:                    return "Sampler2D";
        case xkslang::TypeReflectionType::Sampler3D:                    return "Sampler3D";
        case xkslang::TypeReflectionType::SamplerCube:                  return "SamplerCube";
        case xkslang::TypeReflectionType::UInt:                         return "UInt";
        case xkslang::TypeReflectionType::UInt8:                        return "UInt8";
        case xkslang::TypeReflectionType::Buffer:                       return "Buffer";
        case xkslang::TypeReflectionType::ConstantBuffer:               return "ConstantBuffer";
        case xkslang::TypeReflectionType::TextureBuffer:                return "TextureBuffer";
        case xkslang::TypeReflectionType::Texture1DArray:               return "Texture1DArray";
        case xkslang::TypeReflectionType::Texture2DArray:               return "Texture2DArray";
        case xkslang::TypeReflectionType::Texture2DMultisampled:        return "Texture2DMultisampled";
        case xkslang::TypeReflectionType::Texture2DMultisampledArray:   return "Texture2DMultisampledArray";
        case xkslang::TypeReflectionType::TextureCubeArray:             return "TextureCubeArray";
        case xkslang::TypeReflectionType::Double:                       return "Double";
        case xkslang::TypeReflectionType::RWTexture1D:                  return "RWTexture1D";
        case xkslang::TypeReflectionType::RWTexture1DArray:             return "RWTexture1DArray";
        case xkslang::TypeReflectionType::RWTexture2D:                  return "RWTexture2D";
        case xkslang::TypeReflectionType::RWTexture2DArray:             return "RWTexture2DArray";
        case xkslang::TypeReflectionType::RWTexture3D:                  return "RWTexture3D";
        case xkslang::TypeReflectionType::RWBuffer:                     return "RWBuffer";
        case xkslang::TypeReflectionType::ByteAddressBuffer:            return "ByteAddressBuffer";
        case xkslang::TypeReflectionType::RWByteAddressBuffer:          return "RWByteAddressBuffer";
        case xkslang::TypeReflectionType::StructuredBuffer:             return "StructuredBuffer";
        case xkslang::TypeReflectionType::RWStructuredBuffer:           return "RWStructuredBuffer";
        case xkslang::TypeReflectionType::AppendStructuredBuffer:       return "AppendStructuredBuffer";
        case xkslang::TypeReflectionType::ConsumeStructuredBuffer:      return "ConsumeStructuredBuffer";
        default: return "Unknown";
    }
}