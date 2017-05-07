//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "SpxCompiler.h"

using namespace std;
using namespace xkslang;

bool SpxCompiler::GetBytecodeReflectionData()
{
    bool success;
    success = GetCBufferBytecodeReflectionData();

    if (!success) return error("Failed to get the CBuffer reflection data from the bytecode");

    return true;
}

bool SpxCompiler::GetCBufferBytecodeReflectionData()
{
    bool success = true;

    //=========================================================================================================================
    //get ALL cbuffers
    vector<CBufferTypeData*> listAllCBuffers;
    vector<CBufferTypeData*> vectorCBuffersIds;
    vectorCBuffersIds.resize(bound(), nullptr);

    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj == nullptr) continue;

        if (obj->GetKind() == ObjectInstructionTypeEnum::Type)
        {
            TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
            if (type->IsCBuffer())
            {
                CBufferTypeData* cbufferData = type->GetCBufferData();

#ifdef XKSLANG_DEBUG_MODE
                if (cbufferData->cbufferMembersData != nullptr) { error("a cbuffer members data has already been initialized"); break; }
                if (type->GetId() >= vectorCBuffersIds.size()) { error("cbuffer type id is out of bound. Id: " + to_string(type->GetId())); break; }
                if (cbufferData->cbufferCountMembers <= 0) { error("invalid count members for cbuffer"); break; }
#endif

                cbufferData->cbufferMembersData = new TypeStructMemberArray();
                cbufferData->cbufferMembersData->members.resize(cbufferData->cbufferCountMembers);

                listAllCBuffers.push_back(cbufferData);
                vectorCBuffersIds[type->GetId()] = cbufferData;
            }
        }

        if (errorMessages.size() > 0) success = false;
    }
    if (listAllCBuffers.size() == 0) return success;

    //=========================================================================================================================
    //=========================================================================================================================
    //Retrieve information from the bytecode for all cbuffers and their members
    if (success)
    {
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpMemberAttribute:
                {
                    const spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string attribute = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].attribute = attribute;
                    }
                    break;
                }

                case spv::OpMemberName:
                {
                    spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string memberName = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].declarationName = memberName;
                    }
                    break;
                }

                case spv::OpCBufferMemberProperties:
                {
                    const spv::Id typeId = asId(start + 1);

                    if (vectorCBuffersIds[typeId] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[typeId];

                        spv::XkslPropertyEnum cbufferType = (spv::XkslPropertyEnum)asLiteralValue(start + 2);
                        spv::XkslPropertyEnum cbufferStage = (spv::XkslPropertyEnum)asLiteralValue(start + 3);
                        unsigned int countMembers = asLiteralValue(start + 4);
                        unsigned int remainingBytes = wordCount - 5;

#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferType != cbufferData->cbufferType) { error("Invalid cbuffer type property"); break; }
                        if (countMembers != cbufferData->cbufferCountMembers) { error("Invalid cbuffer count members property"); break; }
#endif

                        if (countMembers != cbufferData->cbufferMembersData->countMembers()) { error("Inconsistent number of members"); break; }
                        if (remainingBytes != countMembers * 2) { error("OpCBufferMemberProperties instruction has an invalid number of bytes"); break; }

                        for (unsigned int m = 0; m < countMembers; m++)
                        {
                            unsigned int k = start + 5 + (m * 2);
                            unsigned int memberSize = asLiteralValue(k);
                            unsigned int memberAlignment = asLiteralValue(k + 1);

                            cbufferData->cbufferMembersData->members[m].memberSize = memberSize;
                            cbufferData->cbufferMembersData->members[m].memberAlignment = memberAlignment;
                        }
                    }
                    break;
                }

                case spv::OpTypeStruct:
                {
                    spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        int countMembers = wordCount - 2;
#ifdef XKSLANG_DEBUG_MODE
                        if (countMembers != cbufferData->cbufferMembersData->members.size()) { error("inconsistent number of members between OpTypeStruct instruction and cbuffer properties"); break; }
#endif
                        for (int m = 0; m < countMembers; ++m)
                        {
                            TypeStructMember& member = cbufferData->cbufferMembersData->members[m];
                            spv::Id memberTypeId = asLiteralValue(start + 2 + m);
                            member.memberTypeId = memberTypeId;

                            //get the member type object
                            TypeInstruction* memberType = GetTypeById(memberTypeId);
                            if (memberType == nullptr) { error("failed to find the member type for memberTypeId: " + to_string(memberTypeId)); break; }
                            member.memberType = memberType;

                            //check if the member type is a resource
                            member.isResourceType = IsResourceType(memberType->opCode);
                        }
                    }
                    break;
                }

                case spv::OpFunction:
                {
                    //all information retrieved at this point: can safely stop here
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }

        if (errorMessages.size() > 0) success = false;
    }

    //=========================================================================================================================
    //get the type for all cbuffer members
    vector<ConstantBufferReflectionDescription> constantBuffers;
    if (success)
    {
        for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
        {
            CBufferTypeData* cbufferData = *itcb;

            constantBuffers.push_back(ConstantBufferReflectionDescription(cbufferData->cbufferName));
            ConstantBufferReflectionDescription& cbufferReflection = constantBuffers.back();

            

            unsigned int countMembers = cbufferData->cbufferCountMembers;
            for (unsigned int m = 0; m < countMembers; ++m)
            {
                TypeStructMember& member = cbufferData->cbufferMembersData->members[m];
    
                TypeReflectionDescription typeReflection;
                if (!GetReflectionTypeForMember(member, typeReflection)) {
                    error("Failed to get the reflection type for the member: " + member.GetDeclarationNameOrSemantic());
                }
            }
    
            if (errorMessages.size() > 0) { success = false; break; }
        }
    }

    if (success)
    {
        //set the size and offset for all members
    }

    //=========================================================================================================================
    //delete allocated data
    for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
    {
        CBufferTypeData* cbufferData = *itcb;
        if (cbufferData->cbufferMembersData != nullptr)
        {
            delete cbufferData->cbufferMembersData;
            cbufferData->cbufferMembersData = nullptr;
        }
    }

    return success;
}

bool SpxCompiler::GetReflectionTypeForMember(TypeStructMember& member, TypeReflectionDescription& typeReflection)
{
    TypeInstruction* memberType = member.memberType;
    if (memberType == nullptr)
    {
        memberType = GetTypeById(member.memberTypeId);
        if (memberType == nullptr) return error("failed to find the member type for memberTypeId: " + to_string(member.memberTypeId));
        member.memberType = memberType;
    }

    if (!GetReflectionTypeFor(memberType, typeReflection, member.attribute, 0)) return false;
    typeReflection.ElementSize = member.memberSize;
    typeReflection.ElementAlignment = member.memberAlignment;

    return true;
}

bool SpxCompiler::GetReflectionTypeFor(TypeInstruction* memberType, TypeReflectionDescription& typeReflection, const string& attribute, int iterationNum)
{
    if (iterationNum >= 5) return error("invalid iteration number");

    switch (memberType->opCode)
    {
        case spv::Op::OpTypeFloat:
        {
            unsigned int width = asLiteralValue(memberType->GetBytecodeStartPosition() + 2);

            switch (width)
            {
                case 16: typeReflection.Set(TypeReflectionClass::Scalar, TypeReflectionType::Float, 1, 1); break;  //should we create a special type?
                case 32: typeReflection.Set(TypeReflectionClass::Scalar, TypeReflectionType::Float, 1, 1); break;
                case 64: typeReflection.Set(TypeReflectionClass::Scalar, TypeReflectionType::Double, 1, 1); break;
                default: return error("OpTypeFloat: invalid type width: " + to_string(width));
            }
            break;
        }

        case spv::Op::OpTypeInt:
        {
            int width = asLiteralValue(memberType->GetBytecodeStartPosition() + 2);
            int sign = asLiteralValue(memberType->GetBytecodeStartPosition() + 3);

            TypeReflectionType reflectionType;
            if (sign == 1)
            {
                reflectionType = TypeReflectionType::Int;
            }
            else
            {
                if (width == 8) reflectionType = TypeReflectionType::UInt8;
                else reflectionType = TypeReflectionType::UInt;
            }

            //Maybe we can differenciate some Int type later (signed/unsigned, int 16,32,64,...)
            typeReflection.Set(TypeReflectionClass::Scalar, reflectionType, 1, 1);
            break;
        }

        case spv::OpTypeBool:
        {
            typeReflection.Set(TypeReflectionClass::Scalar, TypeReflectionType::Bool, 1, 1);
            break;
        }

        case spv::OpTypeVoid:
        {
            typeReflection.Set(TypeReflectionClass::Scalar, TypeReflectionType::Void, 1, 1);
            break;
        }

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
            if (!GetReflectionTypeFor(subElementType, subElementTypeReflection, attribute, iterationNum + 1))
                return error("failed to get the reflection type for the sub-element: " + to_string(subElementTypeId));

            if (subElementTypeReflection.Class != TypeReflectionClass::Vector && subElementTypeReflection.Class != TypeReflectionClass::Color)
                return error("OpTypeMatrix: The sub-element class must be a Vector or Color. class: " + EffectReflection::GetTypeReflectionClassLabel(subElementTypeReflection.Class));
            //if (subElementTypeReflection.Type != TypeReflectionType::Float)
            //    return error("The sub-element type must be Float. Type: " + EffectReflection::GetTypeReflectionTypeLabel(subElementTypeReflection.Type));
            int countColumns = subElementTypeReflection.ColumnCount;
            
            typeReflection.Set(TypeReflectionClass::MatrixColumns, subElementTypeReflection.Type, countRows, countColumns);

            break;
        }

        case spv::Op::OpTypeArray:
        {
            //array size is set with a const instruction: read its value
            spv::Id sizeConstTypeId = asLiteralValue(memberType->GetBytecodeStartPosition() + 3);
            ConstInstruction* constObject = GetConstById(sizeConstTypeId);
            if (constObject == nullptr) return error("cannot get const object for Id: " + to_string(sizeConstTypeId));

            spv::Id constTypeId = constObject->GetTypeId();
            TypeInstruction* constTypeObject = GetTypeById(constTypeId);
            if (constTypeObject == nullptr) return error("no type exist for const typeId: " + to_string(constTypeId));
            spv::Op typeOpCode = asOpCode(constTypeObject->bytecodeStartPosition);
            unsigned int width = asLiteralValue(constTypeObject->bytecodeStartPosition + 2);
            if (typeOpCode != spv::OpTypeInt) return error("the type of the const defining the size of the array has an invalid type (OpTypeInt expected)");
            if (width != 32) return error("the size of the type of the const defining the size of the array is incorrect (32 expected)");
            int arrayCountElems = asLiteralValue(constObject->GetBytecodeStartPosition() + 3);

            //sub-element
            spv::Id subElementTypeId = asId(memberType->GetBytecodeStartPosition() + 2);
            TypeInstruction* subElementType = GetTypeById(subElementTypeId);
            if (subElementType == nullptr) return error("failed to find the element type for id: " + to_string(subElementTypeId));

            if (!GetReflectionTypeFor(subElementType, typeReflection, attribute, iterationNum + 1))
                return error("failed to get the reflection type for the sub-element: " + to_string(subElementTypeId));

            if (typeReflection.Elements != 0) return error("OpTypeArray: sub-element reflection type cannot be another array");
            typeReflection.Elements = arrayCountElems;

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
            if (!GetReflectionTypeFor(subElementType, subElementTypeReflection, attribute, iterationNum + 1))
                return error("failed to get the reflection type for the sub-element: " + to_string(subElementTypeId));

            if (!subElementTypeReflection.isScalarType())
                return error("OpTypeVector: The sub-element class must be scalar. class: " + EffectReflection::GetTypeReflectionClassLabel(subElementTypeReflection.Class));

            TypeReflectionClass vectorClass = TypeReflectionClass::Vector;
            if (subElementTypeReflection.Type == TypeReflectionType::Float && (countElements >= 2 && countElements <= 4)) {
                if (attribute == "Color") {
                    vectorClass = TypeReflectionClass::Color;
                }
            }

            typeReflection.Set(vectorClass, subElementTypeReflection.Type, 1, countElements);

            break;
        }

        case spv::OpTypeStruct:
        {
            return error("OpTypeStruct: not implemented yet");
        }

        default:
            return error("Unprocessed or invalid member type OpCode: " + to_string(memberType->opCode));
    }

    return true;
}
