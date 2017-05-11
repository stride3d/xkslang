//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "SPIRV/doc.h"

#include "SpxCompiler.h"

using namespace std;
using namespace xkslang;

bool SpxCompiler::GetBytecodeReflectionData(EffectReflection& effectReflection)
{
    effectReflection.Clear();

    //Find all output stages entry point
    vector<OutputStageEntryPoint> listEntryPoints;
    {
        unsigned int start = header_size;
        const unsigned int end = spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpEntryPoint:
                {
                    spv::ExecutionModel model = (spv::ExecutionModel)asLiteralValue(start + 1);
                    ShadingStageEnum stage = GetShadingStageForExecutionMode(model);

                    spv::Id entryFunctionId = asId(start + 2);
                    FunctionInstruction* entryFunction = GetFunctionById(entryFunctionId);
                    
                    if (stage == ShadingStageEnum::Undefined) return error("Failed to read the stage from the entry point");
                    if (entryFunction == nullptr) return error("Failed to find the entry function for id: " + to_string(entryFunctionId));

                    listEntryPoints.push_back(OutputStageEntryPoint(stage, entryFunction));
                    break;
                }

                case spv::OpName:
                case spv::OpMemberName:
                case spv::OpFunction:
                case spv::OpConstant:
                {
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }
        if (listEntryPoints.size() == 0) return error("No entry points found in the compiled bytecode");
    }

    bool success;
    success = GetAllCBufferReflectionDataFromBytecode(effectReflection, listEntryPoints);

    if (!success) return error("Failed to get the CBuffer reflection data from the bytecode");

    return true;
}

bool SpxCompiler::GetAllCBufferReflectionDataFromBytecode(EffectReflection& effectReflection, vector<OutputStageEntryPoint>& listEntryPoints)
{
    bool success = true;

    //=========================================================================================================================
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
                if (cbufferData->cbufferCountMembers <= 0) { error("invalid cbuffer member count"); break; }
#endif

                cbufferData->cbufferTypeObject = type;
                cbufferData->cbufferPointerTypeObject = GetTypePointingTo(type);
                cbufferData->cbufferVariableTypeObject = GetVariablePointingTo(cbufferData->cbufferPointerTypeObject);
                if (cbufferData->cbufferVariableTypeObject == nullptr) { error("Failed to find the variable pointing to the cbuffer: " + to_string(type->GetId())); break; }

#ifdef XKSLANG_DEBUG_MODE
                if (type->GetId() >= vectorCBuffersIds.size()) { error("cbuffer type id is out of bound. Id: " + to_string(type->GetId())); break; }
                if (cbufferData->cbufferVariableTypeObject->GetId() >= vectorCBuffersIds.size()) { error("cbuffer variable type id is out of bound. Id: " + to_string(type->GetId())); break; }
#endif

                cbufferData->cbufferMembersData = new TypeStructMemberArray();
                cbufferData->cbufferMembersData->members.resize(cbufferData->cbufferCountMembers);

                listAllCBuffers.push_back(cbufferData);
                vectorCBuffersIds[type->GetId()] = cbufferData;
                //vectorCBuffersIds[cbufferData->cbufferVariableTypeObject->GetId()] = cbufferData;
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
                case spv::OpName:
                {
                    const spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];
                        const string name = literalString(start + 2);
                        cbufferData->cbufferName = name;
                    }
                    break;
                }
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

                case spv::OpCBufferProperties:
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
                        if (remainingBytes != countMembers * 2) { error("OpCBufferProperties instruction has an invalid number of bytes"); break; }

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
    //=========================================================================================================================
    //Set the reflection data for all cbuffer and their members
    if (success)
    {
        effectReflection.ConstantBuffers.resize(listAllCBuffers.size());

        for (unsigned int k = 0; k < listAllCBuffers.size(); k++)
        {
            CBufferTypeData* cbufferData = listAllCBuffers[k];
            unsigned int countMembers = cbufferData->cbufferCountMembers;

            ConstantBufferReflectionDescription& cbufferReflection = effectReflection.ConstantBuffers[k];
            cbufferReflection.CbufferName = cbufferData->cbufferName;
            cbufferReflection.Members.resize(countMembers);

            for (unsigned int mIndex = 0; mIndex < countMembers; ++mIndex)
            {
                TypeStructMember& member = cbufferData->cbufferMembersData->members[mIndex];
                ConstantBufferMemberReflectionDescription& memberReflection = cbufferReflection.Members[mIndex];
                memberReflection.KeyName = member.GetDeclarationNameOrSemantic();

                if (!GetReflectionTypeForMember(member, memberReflection.Type)) {
                    error("Failed to get the reflection type for the member: " + member.GetDeclarationNameOrSemantic());
                    break;
                }

                //compute the member offset (depending on the previous member's offset, its size, plus the new member's alignment
                int memberOffset = 0;
                if (mIndex > 0)
                {
                    ConstantBufferMemberReflectionDescription& previousMemberReflection = cbufferReflection.Members[mIndex - 1];
                    int previousMemberOffset = previousMemberReflection.Offset;
                    int previousMemberSize = previousMemberReflection.Type.ElementSize;

                    memberOffset = previousMemberOffset + previousMemberSize;
                    int memberAlignment = memberReflection.Type.ElementAlignment;
                    //round to pow2
                    memberOffset = (memberOffset + memberAlignment - 1) & (~(memberAlignment - 1));
                }
                memberReflection.Offset = memberOffset;
                cbufferReflection.Size = memberOffset + memberReflection.Type.ElementSize;
            }
    
            if (errorMessages.size() > 0) { success = false; break; }
        }
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //Find which cbuffer resources is used by which output stage
    if (success)
    {
        //we access the cbuffer through their variable
        for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
        {
            CBufferTypeData* cbufferData = *itcb;
            vectorCBuffersIds[cbufferData->cbufferTypeObject->GetId()] = nullptr;
            vectorCBuffersIds[cbufferData->cbufferVariableTypeObject->GetId()] = cbufferData;
        }

        for (unsigned int iStage = 0; iStage < listEntryPoints.size(); ++iStage)
        {
            FunctionInstruction* stageEntryFunction = listEntryPoints[iStage].entryFunction;
            ShadingStageEnum stage = listEntryPoints[iStage].stage;

            //reset the cbuffer flag (we insert a cbuffer once per stage)
            for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
            {
                CBufferTypeData* cbufferData = *itcb;
                cbufferData->tmpFlag = 0;
            }

            //Set all functions flag to 0 (to check a function only once)
            for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
                FunctionInstruction* aFunction = *itsf;
                aFunction->flag1 = 0;
            }

            //Find all functions being called by the stage
            vector<FunctionInstruction*> listAllFunctionsCalledByTheStage;

            vector<FunctionInstruction*> vectorFunctionsToCheck;
            vectorFunctionsToCheck.push_back(stageEntryFunction);
            stageEntryFunction->flag1 = 1;
            while (vectorFunctionsToCheck.size() > 0)
            {
                FunctionInstruction* aFunctionCalled = vectorFunctionsToCheck.back();
                vectorFunctionsToCheck.pop_back();
                listAllFunctionsCalledByTheStage.push_back(aFunctionCalled);

                //check if the function is calling any other function
                unsigned int start = aFunctionCalled->bytecodeStartPosition;
                const unsigned int end = aFunctionCalled->bytecodeEndPosition;
                while (start < end)
                {
                    unsigned int wordCount = asWordCount(start);
                    spv::Op opCode = asOpCode(start);
                    switch (opCode)
                    {
                        case spv::OpAccessChain:
                        {
                            spv::Id structIdAccessed = asId(start + 3);

                            //are we accessing a cbuffer that we just merged?
                            if (vectorCBuffersIds[structIdAccessed] != nullptr)
                            {
                                CBufferTypeData* cbufferData = vectorCBuffersIds[structIdAccessed];
                                if (cbufferData->tmpFlag == 0)
                                {
                                    cbufferData->tmpFlag = 1;
                                    effectReflection.ResourceBindings.push_back(
                                        EffectResourceBindingDescription(
                                            stage,
                                            cbufferData->cbufferName,
                                            EffectParameterReflectionClass::ConstantBuffer,
                                            EffectParameterReflectionType::ConstantBuffer
                                        ));
                                }
                            }
                            break;
                        }

                        case spv::OpFunctionCall:
                        {
                            //pile the function to go check it later
                            spv::Id functionCalledId = asId(start + 3);
                            FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                            if (anotherFunctionCalled == nullptr) {
                                error("Failed to find the function for id: " + to_string(functionCalledId));
                            }
                            else
                            {
                                if (anotherFunctionCalled->flag1 == 0) {
                                    anotherFunctionCalled->flag1 = 1;
                                    vectorFunctionsToCheck.push_back(anotherFunctionCalled); //we'll analyse the function later
                                }
                            }
                            break;
                        }
                    }

                    start += wordCount;
                }
            }

            if (errorMessages.size() > 0) { success = false; break; }
        }
    }

    //=========================================================================================================================
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
                case 16: typeReflection.Set(EffectParameterReflectionClass::Scalar, EffectParameterReflectionType::Float, 1, 1); break;  //should we create a special type?
                case 32: typeReflection.Set(EffectParameterReflectionClass::Scalar, EffectParameterReflectionType::Float, 1, 1); break;
                case 64: typeReflection.Set(EffectParameterReflectionClass::Scalar, EffectParameterReflectionType::Double, 1, 1); break;
                default: return error("OpTypeFloat: invalid type width: " + to_string(width));
            }
            break;
        }

        case spv::Op::OpTypeInt:
        {
            int width = asLiteralValue(memberType->GetBytecodeStartPosition() + 2);
            int sign = asLiteralValue(memberType->GetBytecodeStartPosition() + 3);

            EffectParameterReflectionType reflectionType;
            if (sign == 1)
            {
                reflectionType = EffectParameterReflectionType::Int;
            }
            else
            {
                if (width == 8) reflectionType = EffectParameterReflectionType::UInt8;
                else reflectionType = EffectParameterReflectionType::UInt;
            }

            //Maybe we can differenciate some Int type later (signed/unsigned, int 16,32,64,...)
            typeReflection.Set(EffectParameterReflectionClass::Scalar, reflectionType, 1, 1);
            break;
        }

        case spv::OpTypeBool:
        {
            typeReflection.Set(EffectParameterReflectionClass::Scalar, EffectParameterReflectionType::Bool, 1, 1);
            break;
        }

        case spv::OpTypeVoid:
        {
            typeReflection.Set(EffectParameterReflectionClass::Scalar, EffectParameterReflectionType::Void, 1, 1);
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

            if (subElementTypeReflection.Class != EffectParameterReflectionClass::Vector && subElementTypeReflection.Class != EffectParameterReflectionClass::Color)
                return error("OpTypeMatrix: The sub-element class must be a Vector or Color. class: " + EffectReflection::GetEffectParameterReflectionClassLabel(subElementTypeReflection.Class));
            //if (subElementTypeReflection.Type != EffectParameterReflectionType::Float)
            //    return error("The sub-element type must be Float. Type: " + EffectReflection::GetEffectParameterReflectionTypeLabel(subElementTypeReflection.Type));
            int countColumns = subElementTypeReflection.ColumnCount;
            
            typeReflection.Set(EffectParameterReflectionClass::MatrixColumns, subElementTypeReflection.Type, countRows, countColumns);

            break;
        }

        case spv::Op::OpTypeArray:
        {
            //array size is set with a const instruction: read its value
            spv::Id sizeConstTypeId = asLiteralValue(memberType->GetBytecodeStartPosition() + 3);
            ConstInstruction* constObject = GetConstById(sizeConstTypeId);
            if (constObject == nullptr) return error("cannot get const object for Id: " + to_string(sizeConstTypeId));

            int arrayCountElems;
            if (!GetIntegerConstTypeExpressionValue(constObject, arrayCountElems))
                return error("Failed to get the integer const object literal value for: " + to_string(sizeConstTypeId));

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
                return error("OpTypeVector: The sub-element class must be scalar. class: " + EffectReflection::GetEffectParameterReflectionClassLabel(subElementTypeReflection.Class));

            EffectParameterReflectionClass vectorClass = EffectParameterReflectionClass::Vector;
            if (subElementTypeReflection.Type == EffectParameterReflectionType::Float && (countElements >= 2 && countElements <= 4)) {
                if (attribute == "Color") {
                    vectorClass = EffectParameterReflectionClass::Color;
                }
            }

            typeReflection.Set(vectorClass, subElementTypeReflection.Type, 1, countElements);

            break;
        }

        case spv::OpTypeStruct:
        {
            int wordCount = asWordCount(memberType->GetBytecodeStartPosition());
            int countMembers = wordCount - 2;

            if (countMembers <= 0 || countMembers > 256) return error("Invalid OpTypeStruct size");

            typeReflection.Set(EffectParameterReflectionClass::Struct, EffectParameterReflectionType::Void, 1, 1);
            typeReflection.Members = new TypeMemberReflectionDescription[countMembers];
            typeReflection.CountMembers = countMembers;

            return error("PROUT PROUT Missing struct members size and alignment!!!");

            //unsigned int posElemStart = memberType->GetBytecodeStartPosition() + 2;
            //for (int k = 0; k < countMembers; ++k)
            //{
            //    spv::Id structMemberTypeId = asId(posElemStart + k);
            //    TypeInstruction* structMemberType = GetTypeById(structMemberTypeId);
            //    if (structMemberType == nullptr) return error("failed to find the struct member type for id: " + to_string(structMemberTypeId));
            //
            //    TypeReflectionDescription& memberTypeReflection = typeReflection.Members[k].Type;
            //    if (!GetReflectionTypeFor(structMemberType, memberTypeReflection, attribute, iterationNum + 1))
            //        return error("failed to get the reflection type for the sub-element: " + to_string(structMemberTypeId));
            //
            //    error("PROUT PROUT Missing struct members size and alignment!!!");
            //}

            break;
        }

        default:
            return error(string("Unprocessed or invalid member type OpCode: ") + spv::OpcodeString(memberType->GetOpCode()));
    }

    return true;
}
