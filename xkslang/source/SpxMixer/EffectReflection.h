//
// Copyright (C) 

#ifndef XKSLANG_XKSL_EFFECT_REFLECTION_H__
#define XKSLANG_XKSL_EFFECT_REFLECTION_H__

#include <string>
#include <vector>

#include "../Common/define.h"

namespace xkslang
{

class EffectReflection
{
public:

    class MemberType
    {
    public:
        
        enum class ClassEnum
        {

        };

        //enum class Type
    };

    class ConstantBufferMember
    {
    public:
        std::string keyName;
        int size;
        int offset;
        int alignment;

        MemberType type;

        ConstantBufferMember(const std::string& keyName, int size, int offset, int alignment) : keyName(keyName), size(size), offset(offset), alignment(alignment) {}
    };

    class ConstantBuffer
    {
    public:
        std::string cbufferName;
        std::vector<ConstantBufferMember> members;

        ConstantBuffer(const std::string& cbufferName) : cbufferName(cbufferName) {}

        void AddMember(ConstantBufferMember& member) {
            members .push_back(member);
        }
    };

public:
    EffectReflection() {}

    std::vector<ConstantBuffer> ConstantBuffers;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_EFFECT_REFLECTION_H__
