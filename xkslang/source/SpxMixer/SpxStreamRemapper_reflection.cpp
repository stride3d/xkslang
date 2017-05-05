//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "SPIRV/doc.h"
//#include "SPIRV/disassemble.h"
//#include "SPIRV/SPVRemapper.h"

#include "SpxStreamRemapper.h"

using namespace std;
using namespace xkslang;

bool SpxStreamRemapper::SetReflectionTypeForMember(TypeStructMember& member)
{
    if (member.memberType == nullptr)
    {
        TypeInstruction* memberType = GetTypeById(member.memberTypeId);
        if (memberType == nullptr) { error("failed to find the member type for memberTypeId: " + to_string(member.memberTypeId)); return false; }
        member.memberType = memberType;
    }




    return true;
}