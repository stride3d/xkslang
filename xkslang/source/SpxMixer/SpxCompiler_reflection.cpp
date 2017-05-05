//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "SpxCompiler.h"

using namespace std;
using namespace xkslang;

bool SpxCompiler::GetReflectionTypeForMember(TypeStructMember& member, TypeReflectionDescription& typeReflection)
{
    TypeInstruction* memberType = member.memberType;
    if (memberType == nullptr)
    {
        memberType = GetTypeById(member.memberTypeId);
        if (memberType == nullptr) return error("failed to find the member type for memberTypeId: " + to_string(member.memberTypeId));
        member.memberType = memberType;
    }

    return GetReflectionTypeFor(memberType, typeReflection, 0);
}

bool SpxCompiler::GetReflectionTypeFor(TypeInstruction* memberType, TypeReflectionDescription& typeReflection, int iterationNum)
{
    if (iterationNum >= 5) return error("invalid iteration number");

    switch (memberType->opCode)
    {
        case spv::Op::OpTypeMatrix:
        {
            spv::Id subElementTypeId = asId(memberType->GetBytecodeStartPosition() + 2);
            int countRows = asLiteralValue(memberType->GetBytecodeStartPosition() + 3);

#ifdef XKSLANG_DEBUG_MODE
            if (countRows <= 0) return error("Invalid countRows");
#endif
            TypeInstruction* subElementType = GetTypeById(subElementTypeId);
            if (subElementType == nullptr) return error("failed to find the element type for id: " + to_string(subElementTypeId));

            TypeReflectionDescription subElementTypeReflection;
            if (!GetReflectionTypeFor(subElementType, subElementTypeReflection, iterationNum + 1))
                return error("failed to get the reflection type for the sub-element: " + to_string(subElementTypeId));

            if (!subElementTypeReflection.isScalarType())
                return error("OpTypeVector: The sub-element class must be scalar. class: " + EffectReflection::GetTypeReflectionClassLabel(subElementTypeReflection.Class));
            if (subElementTypeReflection.Type != TypeReflectionType::Float)
                return error("The sub-element type must be Float. Type: " + EffectReflection::GetTypeReflectionTypeLabel(subElementTypeReflection.Type));
            int countColumns = subElementTypeReflection.ColumnCount;
            
            typeReflection.Set(TypeReflectionClass::MatrixColumns, TypeReflectionType::Float, countRows, countColumns);

            break;
        }

        case spv::Op::OpTypeVector:
        {
            spv::Id subElementTypeId = asId(memberType->GetBytecodeStartPosition() + 2);
            int countElements = asLiteralValue(memberType->GetBytecodeStartPosition() + 3);

#ifdef XKSLANG_DEBUG_MODE
            if (countElements <= 0) return error("OpTypeVector: Invalid countElements: " + to_string(countElements));
#endif
            TypeInstruction* subElementType = GetTypeById(subElementTypeId);
            if (subElementType == nullptr) return error("failed to find the element type for id: " + to_string(subElementTypeId));

            TypeReflectionDescription subElementTypeReflection;
            if (!GetReflectionTypeFor(subElementType, subElementTypeReflection, iterationNum + 1))
                return error("failed to get the reflection type for the sub-element: " + to_string(subElementTypeId));

            if (!subElementTypeReflection.isScalarType())
                return error("OpTypeVector: The sub-element class must be scalar. class: " + EffectReflection::GetTypeReflectionClassLabel(subElementTypeReflection.Class));
            //if (subElementTypeReflection.Type != TypeReflectionType::Float) return error("The sub-element type must be Float");
            //int countColumns = subElementTypeReflection.ColumnCount;

            switch (subElementTypeReflection.Type)
            {
                case TypeReflectionType::Float:
                    break;

                default:
                    return error("OpTypeVector: Invalid sub-element type: " + EffectReflection::GetTypeReflectionTypeLabel(subElementTypeReflection.Type));
            }

            typeReflection.Set(TypeReflectionClass::Vector, subElementTypeReflection.Type, 1, countElements);

            break;
        }

        default:
            return error("Invalid member type OpCode: " + to_string(memberType->opCode));
    }

    return true;
}