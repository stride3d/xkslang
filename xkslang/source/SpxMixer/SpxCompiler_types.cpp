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

bool SpxCompiler::IsResourceType(const spv::Op& opCode)
{
    switch (opCode)
    {
        case spv::OpTypeImage:
        case spv::OpTypeSampler:
        case spv::OpTypeSampledImage:
            return true;
    }

    return false;
}

bool SpxCompiler::IsMatrixArrayType(TypeInstruction* type)
{
    if (type->GetOpCode() != spv::OpTypeArray) return false;

    spv::Id arrayElementTypeId = asId(type->GetBytecodeStartPosition() + 2);
    TypeInstruction* elemType = GetTypeById(arrayElementTypeId);
    if (elemType == nullptr) return error("failed to find the vector element type for id: " + to_string(arrayElementTypeId));

    return (elemType->GetOpCode() == spv::OpTypeMatrix);
}

bool SpxCompiler::IsScalarType(const spv::Op& opCode)
{
    switch (opCode)
    {
        case spv::OpTypeInt:
        case spv::OpTypeFloat:
        case spv::OpTypeBool:
           return true;
    }

    return false;
}

int SpxCompiler::GetVectorTypeCountElements(TypeInstruction* vectorType)
{
    if (vectorType->GetOpCode() != spv::OpTypeVector) return -1;
    int countElems = asLiteralValue(vectorType->GetBytecodeStartPosition() + 3);
    return countElems;
}

bool SpxCompiler::GetIntegerConstTypeExpressionValue(ConstInstruction* constObject, int& constValue)
{
    spv::Id constTypeId = constObject->GetTypeId();
    TypeInstruction* constTypeObject = GetTypeById(constTypeId);
    if (constTypeObject == nullptr) return error("no type exist for const typeId: " + to_string(constTypeId));

    spv::Op typeOpCode = asOpCode(constTypeObject->bytecodeStartPosition);
    unsigned int width = asLiteralValue(constTypeObject->bytecodeStartPosition + 2);
    if (typeOpCode != spv::OpTypeInt) return error("the type of the const defining the size of the array has an invalid type (OpTypeInt expected)");
    if (width != 32) return error("the size of the type of the const defining the size of the array is incorrect (32 expected)");

    constValue = asLiteralValue(constObject->GetBytecodeStartPosition() + 3);
    return true;
}

spv::Id SpxCompiler::GetOrCreateTypeDefaultConstValue(spv::Id& newId, TypeInstruction* type, const vector<ConstInstruction*>& listAllConsts,
    vector<spv::Instruction>& listNewConstInstructionsToAdd, int iterationCounter)
{
    if (iterationCounter > 10) { error("Too many nested type (infinite type redirection in the bytecode?)"); return 0; }

    const spv::Op& typeOpCode = type->opCode;
    const spv::Id& typeId = type->GetId();
    spv::Id typeConstId = 0;

#ifdef XKSLANG_DEBUG_MODE
    //some sanity check
    if (asOpCode(type->GetBytecodeStartPosition()) != typeOpCode) {error("Corrupted bytecode: type opcode is not matching the bytecode"); return 0;}
#endif

    spv::Instruction newConstInstruction(spv::OpUndef);

    switch (typeOpCode)
    {
        case spv::OpTypeInt:
        case spv::OpTypeFloat:
        {
            unsigned int width = asLiteralValue(type->GetBytecodeStartPosition() + 2);
            unsigned int countOperands = (width == 16? 1 : width >> 5);

#ifdef XKSLANG_DEBUG_MODE
            if (!(width == 16 || (width % 32) == 0)) { error("Invalid width size for the type"); return 0; }
#endif

            newConstInstruction.SetResultTypeAndOpCode(newId++, typeId, spv::OpConstant);
            for (unsigned int k = 0; k < countOperands; k++) newConstInstruction.addImmediateOperand(0);
        }
        break;

        case spv::OpTypeBool:
        {
            newConstInstruction.SetResultTypeAndOpCode(newId++, typeId, spv::OpConstantFalse);
        }
        break;

        case spv::OpTypeMatrix:
        case spv::OpTypeVector:
        case spv::OpTypeArray:
        {
            spv::Id vectorElementTypeId = asId(type->GetBytecodeStartPosition() + 2);

            int countElems = 0;
            if (typeOpCode == spv::OpTypeArray)
            {
                //array size is set with a const instruction
                spv::Id sizeConstTypeId = asLiteralValue(type->GetBytecodeStartPosition() + 3);
                ConstInstruction* constObject = GetConstById(sizeConstTypeId);
                if (constObject == nullptr) { error("cannot get const object for Id: " + to_string(sizeConstTypeId)); return 0; }

                if (!GetIntegerConstTypeExpressionValue(constObject, countElems)) { error("Failed to get the integer const object literal value for: " + to_string(sizeConstTypeId)); return 0; }
            }
            else
            {
                countElems = asLiteralValue(type->GetBytecodeStartPosition() + 3);
            }

#ifdef XKSLANG_DEBUG_MODE
            if (countElems <= 0 || countElems > compilerSettings_maxArraySize) { error("Invalid countElems size"); return 0; }
#endif
            TypeInstruction* vectorElemType = GetTypeById(vectorElementTypeId);
            if (vectorElemType == nullptr) { error("failed to find the vector type for id: " + to_string(vectorElementTypeId)); return 0; }

            spv::Id elemConstValueId = GetOrCreateTypeDefaultConstValue(newId, vectorElemType, listAllConsts, listNewConstInstructionsToAdd, iterationCounter + 1);
            if (elemConstValueId == 0) return 0;
            
            newConstInstruction.SetResultTypeAndOpCode(newId++, typeId, spv::OpConstantComposite);
            for (int k = 0; k < countElems; ++k) newConstInstruction.addIdOperand(elemConstValueId);
        }
        break;

        case spv::OpTypeStruct:
        {
            int wordCount = asWordCount(type->GetBytecodeStartPosition());
            int countElems = wordCount - 2;

#ifdef XKSLANG_DEBUG_MODE
            if (countElems <= 0) { error("Invalid OpTypeStruct size"); return 0; }
#endif

            unsigned int posElemStart = type->GetBytecodeStartPosition() + 2;
            vector<spv::Id> vecStructElemsConstsId;
            for (int k = 0; k < countElems; ++k)
            {
                spv::Id structElemTypeId = asId(posElemStart + k);
                TypeInstruction* structElemType = GetTypeById(structElemTypeId);
                if (structElemType == nullptr) { error("failed to find the struct element type for id: " + to_string(structElemTypeId)); return 0; }

                spv::Id elemConstValueId = GetOrCreateTypeDefaultConstValue(newId, structElemType, listAllConsts, listNewConstInstructionsToAdd, iterationCounter + 1);
                if (elemConstValueId == 0) return 0;
                vecStructElemsConstsId.push_back(elemConstValueId);
            }

            newConstInstruction.SetResultTypeAndOpCode(newId++, typeId, spv::OpConstantComposite);
            for (unsigned int k = 0; k < vecStructElemsConstsId.size(); ++k) newConstInstruction.addIdOperand(vecStructElemsConstsId[k]);
        }
        break;

        default:       
            error("Cannot create a default const value, unprocessed or invalid type");
            break;
    }

    if (newConstInstruction.getOpCode() == spv::OpUndef)
    {
        error("No const instruction has been created");
        return 0;
    }

    //check if a duplicate const already exists in our list of consts
    ConstInstruction* existingConst = FindConstFromList(listAllConsts, newConstInstruction.getOpCode(), newConstInstruction.getTypeId(), newConstInstruction.GetOperands());
    if (existingConst != nullptr)
    {
        newId--;
        typeConstId = existingConst->GetResultId();
    }
    else
    {
        //check that the new const has not already been recursively created in this function

        int index = -1;
        for (unsigned int n = 0; n < listNewConstInstructionsToAdd.size(); n++)
        {
            const spv::Instruction& anInstruction = listNewConstInstructionsToAdd[n];
            if (anInstruction.getOpCode() == newConstInstruction.getOpCode() && anInstruction.getTypeId() == newConstInstruction.getTypeId())
            {
                bool sameInstruction = true;
                unsigned int numOperands = newConstInstruction.getNumOperands();
                if (anInstruction.getNumOperands() == numOperands)
                {
                    for (unsigned int op = 0; op < numOperands; ++op)
                    {
                        if (anInstruction.getImmediateOperand(op) != newConstInstruction.getImmediateOperand(op)) {
                            sameInstruction = false;
                            break;
                        }
                    }
                }

                if (sameInstruction)
                {
                    index = n;
                    break;
                }
            }
        }

        if (index != -1)
        {
            newId--;
            typeConstId = listNewConstInstructionsToAdd[index].getResultId();
        }
        else
        {
            listNewConstInstructionsToAdd.push_back(newConstInstruction);
            typeConstId = newConstInstruction.getResultId();
        }
    }

    return typeConstId;
}

//Rules are inspired from glslang TIntermediate::getBaseAlignment function
bool SpxCompiler::GetTypeReflectionDescription(TypeInstruction* type, bool isRowMajor, string* memberAttribute, TypeReflectionDescription& typeReflection,
    const vector<unsigned int>* listStartPositionOfAllMemberDecorateInstructions, int iterationCounter)
{
    if (iterationCounter > 10) return error("Too many recursive iterations (infinite type redirection in the bytecode?)");

    int memberSize = 0;
    int memberAlignment = 0;
    int arrayStride = 0;
    int matrixStride = 0;
    int countRows = 1;
    int countCols = 1;
    int countElementsInArray = 0;
    EffectParameterReflectionClass memberClass = EffectParameterReflectionClass::Undefined;
    EffectParameterReflectionType memberType = EffectParameterReflectionType::Undefined;

    //For struct members
    TypeMemberReflectionDescription* structMembers = nullptr;
    int countStructMembers = 0;

    const spv::Op typeOpCode = type->GetOpCode();
    switch (typeOpCode)
    {
        //===================================================================================
        //Resources
        case spv::OpTypeSampler:
        {
            memberClass = EffectParameterReflectionClass::Sampler;
            memberType = EffectParameterReflectionType::Sampler;
            memberSize = 4;
            memberAlignment = 4;

            break;
        }

        case spv::OpTypeImage:
        {
            int imageDim = asLiteralValue(type->GetBytecodeStartPosition() + 3);
            
            switch (imageDim)
            {
                case 0: memberType = EffectParameterReflectionType::Texture1D;  break;
                case 1: memberType = EffectParameterReflectionType::Texture2D;  break;
                case 2: memberType = EffectParameterReflectionType::Texture3D;  break;
                case 3: memberType = EffectParameterReflectionType::TextureCube;  break;

				case 5: memberType = EffectParameterReflectionType::TextureBuffer;  break;   //TODO: check if it's the correct type
                //TODO: check more cases

                default: return error("Invalid OpTypeImage dimension: " + to_string(imageDim));
            }

            memberClass = EffectParameterReflectionClass::ShaderResourceView;
            memberSize = 4;
            memberAlignment = 4;

            break;
        }

        //===================================================================================
        //Scalars
        case spv::OpTypeVoid:
        {
            memberClass = EffectParameterReflectionClass::Scalar;
            memberType = EffectParameterReflectionType::Void;
            memberSize = 4;
            memberAlignment = 4;
            break;
        }

        case spv::OpTypeBool:
        {
            memberClass = EffectParameterReflectionClass::Scalar;
            memberType = EffectParameterReflectionType::Bool;
            memberSize = 4;
            memberAlignment = 4;
            break;
        }

        case spv::OpTypeInt:
        {
            unsigned int width = asLiteralValue(type->GetBytecodeStartPosition() + 2);
            int sign = asLiteralValue(type->GetBytecodeStartPosition() + 3);

            memberClass = EffectParameterReflectionClass::Scalar;
            if (sign == 1) {
                memberType = EffectParameterReflectionType::Int;
            }
            else {
                if (width == 8) memberType = EffectParameterReflectionType::UInt8;
                else memberType = EffectParameterReflectionType::UInt;
            }

            int nbByte = width >> 3;
            memberSize = nbByte;
            memberAlignment = nbByte;

#ifdef XKSLANG_DEBUG_MODE
            if ((width % 8) != 0) return error("Invalid width size for the type");
#endif

            break;
        }

        case spv::OpTypeFloat:
        {
            unsigned int width = asLiteralValue(type->GetBytecodeStartPosition() + 2);

            memberClass = EffectParameterReflectionClass::Scalar;
            switch (width)
            {
                case 16: memberType = EffectParameterReflectionType::Float; break;  //should we create a special type?
                case 32: memberType = EffectParameterReflectionType::Float; break;
                case 64: memberType = EffectParameterReflectionType::Double; break;
                default: return error("OpTypeFloat: invalid type width: " + to_string(width));
            }

            int nbByte = width >> 3;
            memberSize = nbByte;
            memberAlignment = nbByte;
            break;
        }

        //===================================================================================
        // vector & array
        case spv::OpTypeVector:
        {
            spv::Id vectorElementTypeId = asId(type->GetBytecodeStartPosition() + 2);
            int countElements = asLiteralValue(type->GetBytecodeStartPosition() + 3);

            TypeInstruction* elemType = GetTypeById(vectorElementTypeId);
            if (elemType == nullptr) return error("failed to find the vector element type for id: " + to_string(vectorElementTypeId));

#ifdef XKSLANG_DEBUG_MODE
            if (countElements <= 0) return error("Invalid vector countElems");
            if (!IsScalarType(elemType->GetOpCode())) return error("the vector element type must be scalar");
#endif

            TypeReflectionDescription subElementReflection;
            if (!GetTypeReflectionDescription(elemType, isRowMajor, memberAttribute, subElementReflection, listStartPositionOfAllMemberDecorateInstructions, iterationCounter+1))
                return error("Failed to get the reflection data for the vector sub-element type");

            if (!subElementReflection.isScalarType())
                return error("OpTypeVector: The sub-element class must be scalar. class: " + EffectReflection::GetEffectParameterReflectionClassLabel(subElementReflection.Class));

            if (countElements == 2) {
                memberSize = subElementReflection.Size * 2;
                memberAlignment = subElementReflection.Alignment * 2;
            }
            else {
                memberSize = subElementReflection.Size * countElements;
                memberAlignment = subElementReflection.Alignment * 4;
            }

            memberType = subElementReflection.Type;
            memberClass = EffectParameterReflectionClass::Vector;
            if (subElementReflection.Type == EffectParameterReflectionType::Float && (countElements >= 2 && countElements <= 4) && memberAttribute != nullptr) {
                if (*memberAttribute == "Color") {
                    memberClass = EffectParameterReflectionClass::Color;
                }
            }

            countRows = 1;
            countCols = countElements;

            break;
        }

        case spv::OpTypeArray:
        {
            spv::Id arrayElementTypeId = asId(type->GetBytecodeStartPosition() + 2);
            TypeInstruction* elemType = GetTypeById(arrayElementTypeId);
            if (elemType == nullptr) return error("failed to find the vector element type for id: " + to_string(arrayElementTypeId));

            //array size is set with a const instruction
            spv::Id sizeConstTypeId = asLiteralValue(type->GetBytecodeStartPosition() + 3);
            ConstInstruction* constObject = GetConstById(sizeConstTypeId);
            if (constObject == nullptr) return error("cannot get const object for Id: " + to_string(sizeConstTypeId));

            int arrayCountElems;
            if (!GetIntegerConstTypeExpressionValue(constObject, arrayCountElems))
                return error("Failed to get the integer const object literal value for: " + to_string(sizeConstTypeId));

#ifdef XKSLANG_DEBUG_MODE
            if (arrayCountElems <= 0 || arrayCountElems >= compilerSettings_maxArraySize) return error("Invalid vector countElems");
#endif

            TypeReflectionDescription subElementReflection;
            if (!GetTypeReflectionDescription(elemType, isRowMajor, memberAttribute, subElementReflection, listStartPositionOfAllMemberDecorateInstructions, iterationCounter + 1))
                return error("Failed to get the reflection data for the array element type");
            if (subElementReflection.ArrayElements != 0) return error("OpTypeArray: sub-element reflection type cannot be another array");

            int subElementAlign = subElementReflection.Alignment;
            if (compilerSettings_useStd140Rules) subElementAlign = std::max(compilerSettings_baseAlignmentVec4Std140, subElementAlign);

            matrixStride = subElementReflection.MatrixStride;
            arrayStride = RoundToPow2(subElementReflection.Size, subElementAlign);
            memberSize = arrayStride * arrayCountElems;
            memberAlignment = subElementAlign;

            memberType = subElementReflection.Type;
            memberClass = subElementReflection.Class;
            countRows = subElementReflection.RowCount;
            countCols = subElementReflection.ColumnCount;
            countElementsInArray = arrayCountElems;

            break;
        }

        //===================================================================================
        // matrix
        case spv::OpTypeMatrix:
        {
            spv::Id matrixElementTypeId = asId(type->GetBytecodeStartPosition() + 2);
            int countVectors = asLiteralValue(type->GetBytecodeStartPosition() + 3);
            
            TypeInstruction* matrixVectorType = GetTypeById(matrixElementTypeId);
            if (matrixVectorType == nullptr) return error("failed to find the matrix element type for id: " + to_string(matrixElementTypeId));

            spv::Id vectorElementTypeId = asId(matrixVectorType->GetBytecodeStartPosition() + 2);
            int vectorCountElems = asLiteralValue(matrixVectorType->GetBytecodeStartPosition() + 3);
            TypeInstruction* elemType = GetTypeById(vectorElementTypeId);
            if (elemType == nullptr) return error("failed to find the vector element type for id: " + to_string(vectorElementTypeId));

#ifdef XKSLANG_DEBUG_MODE
            if (countVectors <= 0 || countVectors > compilerSettings_maxMatrixSize) return error("Invalid matrix countElems");
            if (vectorCountElems <= 0 || vectorCountElems > compilerSettings_maxMatrixSize) return error("Invalid matrix vector countElems");
            if (!IsScalarType(elemType->GetOpCode())) return error("the matrix's element type must be scalar");
#endif

            TypeReflectionDescription subElementReflection;
            if (!GetTypeReflectionDescription(elemType, isRowMajor, memberAttribute, subElementReflection, listStartPositionOfAllMemberDecorateInstructions, iterationCounter + 1))
                return error("Failed to get the reflection data for the matrix's element type");
            int subElementSize = subElementReflection.Size;
            int subElementAlign = subElementReflection.Alignment;

            memberType = subElementReflection.Type;

            if (isRowMajor)
            {
                memberClass = EffectParameterReflectionClass::MatrixRows;
                countCols = countVectors;
                countRows = vectorCountElems;
            }
            else
            {
                memberClass = EffectParameterReflectionClass::MatrixColumns;
                countCols = vectorCountElems;
                countRows = countVectors;
            }

            int rowVectorSize;
            int rowVectorAlignment;
            if (countCols == 2) {
                rowVectorSize = subElementSize * 2;
                rowVectorAlignment = subElementAlign * 2;
            }
            else {
                rowVectorSize = subElementSize * countCols;
                rowVectorAlignment = subElementAlign * 4;
            }

            if (compilerSettings_useStd140Rules) rowVectorAlignment = std::max(compilerSettings_baseAlignmentVec4Std140, rowVectorAlignment);
            rowVectorSize = RoundToPow2(rowVectorSize, rowVectorAlignment);
            if (rowVectorSize < 0) return error("Failed to compute the matrix row vector size");
            
            matrixStride = rowVectorSize;
            memberSize = rowVectorSize * countRows;
            memberAlignment = rowVectorAlignment;
            
            break;
        }

        //===================================================================================
        // struct
        case spv::OpTypeStruct:
        {
            int wordCount = asWordCount(type->GetBytecodeStartPosition());
            int countMembers = wordCount - 2;
            spv::Id structTypeId = type->GetId();

#ifdef XKSLANG_DEBUG_MODE
            if (countMembers <= 0 || countMembers > compilerSettings_maxStructMembers) { error("Invalid OpTypeStruct size"); return 0; }
#endif
            countStructMembers = countMembers;
            structMembers = new TypeMemberReflectionDescription[countStructMembers];

            memberSize = 0;
            int maxAlignment = compilerSettings_useStd140Rules ? compilerSettings_baseAlignmentVec4Std140 : 0;
            unsigned int posElemStart = type->GetBytecodeStartPosition() + 2;
            for (int m = 0; m < countMembers; ++m)
            {
                TypeMemberReflectionDescription& structMember = structMembers[m];

                spv::Id structElemTypeId = asId(posElemStart + m);
                TypeInstruction* structElemType = GetTypeById(structElemTypeId);
                if (structElemType == nullptr) { error("failed to find the struct element type for id: " + to_string(structElemTypeId)); break; }

                if (IsMatrixType(structElemType) || IsMatrixArrayType(structElemType))
                {
                    if (listStartPositionOfAllMemberDecorateInstructions == nullptr) {
                        error("listStartPositionOfAllMemberDecorateInstructions is null"); break;
                    }

                    //Find the member's matrix layout type (RowMajor or ColMajor): we look for it in the list of all memberDecorates
                    bool found = false;
                    for (auto itmb = listStartPositionOfAllMemberDecorateInstructions->begin(); itmb != listStartPositionOfAllMemberDecorateInstructions->end(); itmb++)
                    {
                        unsigned int start = *itmb;
                        const spv::Op opCode = asOpCode(start);
                        if (opCode == spv::OpMemberDecorate)
                        {
                            const spv::Id id = asId(start + 1);
                            const unsigned int index = asLiteralValue(start + 2);
                            if (id == structTypeId && m == index)
                            {
                                const spv::Decoration dec = (spv::Decoration)asLiteralValue(start + 3);
                                if (dec == spv::Decoration::DecorationRowMajor) {
                                    isRowMajor = true;
                                    found = true;
                                    break;
                                }
                                else if (dec == spv::Decoration::DecorationColMajor) {
                                    isRowMajor = false;
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!found) {
                        error("A struct member has no layout decoration for its matrix (or matrix array) type");
                        break;
                    }
                }

                TypeReflectionDescription& subElementReflection = structMember.Type;
                if (!GetTypeReflectionDescription(structElemType, isRowMajor, memberAttribute, subElementReflection, listStartPositionOfAllMemberDecorateInstructions, iterationCounter + 1))
                {
                    error("Failed to get the reflection data for the struct's member type");
                    break;
                }

                maxAlignment = std::max(maxAlignment, subElementReflection.Alignment);

                //Add some padding to the size, depending on the member alignment
                memberSize = RoundToPow2(memberSize, subElementReflection.Alignment);

                //set the sub-member offset
                structMember.Offset = memberSize;

                memberSize += subElementReflection.Size;
            }

            if (errorMessages.size() > 0)
            {
                if (structMembers != nullptr) delete[] structMembers;
                return error("Failed to get the struct members reflection data");
            }

            //do we add or not the last padding within the struct size?
            memberSize = RoundToPow2(memberSize, maxAlignment);
            memberAlignment = maxAlignment;

            memberClass = EffectParameterReflectionClass::Struct;
            memberType = EffectParameterReflectionType::Void;
            break;
        }

        default: return error(string("Unknowns type OpCode: ") + spv::OpcodeString(type->GetOpCode()));
    }

    typeReflection.Set(type->GetResultId(), memberClass, memberType, countRows, countCols, memberSize, memberAlignment, arrayStride, matrixStride, countElementsInArray);
    if (structMembers != nullptr)
    {
        typeReflection.SetStructMembers(structMembers, countStructMembers);
    }

    return true;
}


