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
            if (!(width == 16 || width % 32 == 0)) { error("Invalid width size for the type"); return 0; }
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
            if (countElems <= 0 || countElems > maxArraySize) { error("Invalid countElems size"); return 0; }
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
bool SpxCompiler::GetTypeObjectBaseSizeAndAlignment(TypeInstruction* type, bool isRowMajor, int& size, int& alignment, int& stride, int iterationCounter)
{
    if (iterationCounter > 10) return error("Too many recursive iterations (infinite type redirection in the bytecode?)");

    stride = 0;
    switch (type->GetOpCode())
    {
        //===================================================================================
        //Scalars and pointer
        case spv::OpTypeBool:
        case spv::OpTypeVoid:
        case spv::OpTypeSampler:
        case spv::OpTypeImage:
        {
            size = 4;
            alignment = 4;
            break;
        }

        case spv::OpTypeInt:
        case spv::OpTypeFloat:
        {
            unsigned int width = asLiteralValue(type->GetBytecodeStartPosition() + 2);
            //unsigned int countOperands = (width == 16 ? 1 : width >> 5);
#ifdef XKSLANG_DEBUG_MODE
            if (!(width == 16 || width % 32 == 0)) return error("Invalid width size for the type");
#endif
            int nbByte = width >> 3;
            size = nbByte;
            alignment = nbByte;
            break;
        }

        case spv::OpTypeVector:
        {
            spv::Id vectorElementTypeId = asId(type->GetBytecodeStartPosition() + 2);
            int countElems = asLiteralValue(type->GetBytecodeStartPosition() + 3);

            TypeInstruction* elemType = GetTypeById(vectorElementTypeId);
            if (elemType == nullptr) return error("failed to find the vector element type for id: " + to_string(vectorElementTypeId));

#ifdef XKSLANG_DEBUG_MODE
            if (countElems <= 0) return error("Invalid vector countElems");
            if (!IsScalarType(elemType->GetOpCode())) return error("the vector element type must be scalar");
#endif

            int subElemSize, subElemAlign, subElemStride;
            if (!GetTypeObjectBaseSizeAndAlignment(elemType, isRowMajor, subElemSize, subElemAlign, subElemStride, iterationCounter+1))
                return error("Failed to get the size and alignment for the vector sub-element type");

            if (countElems == 2) {
                size = subElemSize * 2;
                alignment = subElemAlign * 2;
            }
            else {
                size = subElemSize * countElems;
                alignment = subElemAlign * 4;
            }

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

            int countElems;
            if (!GetIntegerConstTypeExpressionValue(constObject, countElems))
                return error("Failed to get the integer const object literal value for: " + to_string(sizeConstTypeId));

#ifdef XKSLANG_DEBUG_MODE
            if (countElems <= 0 || countElems >= maxArraySize) return error("Invalid vector countElems");
#endif

            int subElemSize, subElemAlign, subElemStride;
            if (!GetTypeObjectBaseSizeAndAlignment(elemType, isRowMajor, subElemSize, subElemAlign, subElemStride, iterationCounter + 1))
                return error("Failed to get the size and alignment for the array element type");

            if (useStd140Rules) subElemAlign = std::max(baseAlignmentVec4Std140, subElemAlign);
            subElemSize = RoundToPow2(subElemSize, subElemAlign);

            stride = subElemSize;
            size = subElemSize * countElems;
            alignment = subElemAlign;

            break;
        }

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
            if (countVectors <= 0 || countVectors > maxMatrixSize) return error("Invalid matrix countElems");
            if (vectorCountElems <= 0 || vectorCountElems > maxMatrixSize) return error("Invalid matrix vector countElems");
            if (!IsScalarType(elemType->GetOpCode())) return error("the matrix's element type must be scalar");
#endif

            int subElemSize, subElemAlign, subElemStride;
            if (!GetTypeObjectBaseSizeAndAlignment(elemType, isRowMajor, subElemSize, subElemAlign, subElemStride, iterationCounter + 1))
                return error("Failed to get the size and alignment for the matrix's element type");

            int countElementsPerRows, countColumns;
            if (isRowMajor) {
                countElementsPerRows = countVectors;
                countColumns = vectorCountElems;
            }
            else {
                countElementsPerRows = vectorCountElems;
                countColumns = countVectors;
            }

            int rowVectorSize;
            int rowVectorAlignment;
            if (countElementsPerRows == 2) {
                rowVectorSize = subElemSize * 2;
                rowVectorAlignment = subElemAlign * 2;
            }
            else {
                rowVectorSize = subElemSize * countElementsPerRows;
                rowVectorAlignment = subElemAlign * 4;
            }

            if (useStd140Rules) rowVectorAlignment = std::max(baseAlignmentVec4Std140, rowVectorAlignment);
            rowVectorSize = RoundToPow2(rowVectorSize, rowVectorAlignment);
            if (rowVectorSize < 0) return error("Failed to compute the matrix row vector size");
            
            stride = rowVectorSize;
            size = rowVectorSize * countColumns;
            alignment = rowVectorAlignment;

            break;
        }

        case spv::OpTypeStruct:
        {
            int wordCount = asWordCount(type->GetBytecodeStartPosition());
            int countElems = wordCount - 2;

#ifdef XKSLANG_DEBUG_MODE
            if (countElems <= 0) { error("Invalid OpTypeStruct size"); return 0; }
#endif
            size = 0;
            int maxAlignment = useStd140Rules ? baseAlignmentVec4Std140 : 0;
            unsigned int posElemStart = type->GetBytecodeStartPosition() + 2;
            for (unsigned int m = 0; m < countElems; ++m)
            {
                spv::Id structElemTypeId = asId(posElemStart + m);
                TypeInstruction* structElemType = GetTypeById(structElemTypeId);
                if (structElemType == nullptr) return error("failed to find the struct element type for id: " + to_string(structElemTypeId));

                if (IsMatrixType(structElemType->GetOpCode())) return error("Got to find the matric RowMajor decorate!!");

                int subElemSize, subElemAlign, subElemStride;
                if (!GetTypeObjectBaseSizeAndAlignment(structElemType, isRowMajor, subElemSize, subElemAlign, subElemStride, iterationCounter + 1))
                    return error("Failed to get the size and alignment for the struct's member type");

                maxAlignment = std::max(maxAlignment, subElemAlign);
                size = RoundToPow2(size, subElemAlign);
                size += subElemSize;
            }

            // The structure may have padding at the end;
            // the base offset of the member following the sub-structure is rounded up to the next multiple of the base alignment of the structure.
            size = RoundToPow2(size, maxAlignment);
            alignment = maxAlignment;
            break;
        }

        default:
            return error(string("Unknowns type OpCode: ") + spv::OpcodeString(type->GetOpCode()));
    }

    return true;
}


