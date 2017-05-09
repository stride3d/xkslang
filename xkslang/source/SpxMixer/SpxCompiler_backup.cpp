//
// Copyright (C)



#if 0



///// Cloned from glslang
// When using the std140 storage layout, structures will be laid out in buffer
// storage with its members stored in monotonically increasing order based on their
// location in the declaration. A structure and each structure member have a base
// offset and a base alignment, from which an aligned offset is computed by rounding
// the base offset up to a multiple of the base alignment. The base offset of the first
// member of a structure is taken from the aligned offset of the structure itself. The
// base offset of all other structure members is derived by taking the offset of the
// last basic machine unit consumed by the previous member and adding one. Each
// structure member is stored in memory at its aligned offset. The members of a top-
// level uniform block are laid out in buffer storage by treating the uniform block as
// a structure with a base offset of zero.
//
//   1. If the member is a scalar consuming N basic machine units, the base alignment is N.
//
//   2. If the member is a two- or four-component vector with components consuming N basic
//      machine units, the base alignment is 2N or 4N, respectively.
//
//   3. If the member is a three-component vector with components consuming N
//      basic machine units, the base alignment is 4N.
//
//   4. If the member is an array of scalars or vectors, the base alignment and array
//      stride are set to match the base alignment of a single array element, according
//      to rules (1), (2), and (3), and rounded up to the base alignment of a vec4. The
//      array may have padding at the end; the base offset of the member following
//      the array is rounded up to the next multiple of the base alignment.
//
//   5. If the member is a column-major matrix with C columns and R rows, the
//      matrix is stored identically to an array of C column vectors with R
//      components each, according to rule (4).
//
//   6. If the member is an array of S column-major matrices with C columns and
//      R rows, the matrix is stored identically to a row of S X C column vectors
//      with R components each, according to rule (4).
//
//   7. If the member is a row-major matrix with C columns and R rows, the matrix
//      is stored identically to an array of R row vectors with C components each,
//      according to rule (4).
//
//   8. If the member is an array of S row-major matrices with C columns and R
//      rows, the matrix is stored identically to a row of S X R row vectors with C
//      components each, according to rule (4).
//
//   9. If the member is a structure, the base alignment of the structure is N , where
//      N is the largest base alignment value of any    of its members, and rounded
//      up to the base alignment of a vec4. The individual members of this substructure
//      are then assigned offsets by applying this set of rules recursively,
//      where the base offset of the first member of the sub-structure is equal to the
//      aligned offset of the structure. The structure may have padding at the end;
//      the base offset of the member following the sub-structure is rounded up to
//      the next multiple of the base alignment of the structure.
//
//   10. If the member is an array of S structures, the S elements of the array are laid
//       out in order, according to rule (9).
//
//   Assuming, for rule 10:  The stride is the same as the size of an element.
bool SpxCompiler::SetNewTypeObjectSizeAndAlignment(TypeInstruction* type)
{
    switch (type->GetOpCode())
    {
        //some default values (void has a size (inspired from glslang))
        case spv::OpTypeVoid:
        case spv::OpTypePointer:
        case spv::OpTypeFunction:
        {
            type->SetSizeAndAlignment(4, 4);
            break;
        }

        // Vectors: rules 2 and 3
        case spv::OpTypeVector:
        {
            spv::Id vectorElementTypeId = asId(type->GetBytecodeStartPosition() + 2);
            int countElems = asLiteralValue(type->GetBytecodeStartPosition() + 3);

            TypeInstruction* elemType = GetTypeById(vectorElementTypeId);
            if (elemType == nullptr) return error("failed to find the vector type for id: " + to_string(vectorElementTypeId));

#ifdef XKSLANG_DEBUG_MODE
            if (countElems <= 0) return error("Invalid vector countElems");
            if (!elemType->HasValidSizeAndAlignment()) return error("the vector element type has an invalid size or alignment: " + to_string(vectorElementTypeId));
#endif

            if (countElems == 2) type->SetSizeAndAlignment(elemType->typeSize * 2, elemType->typeAlignment * 2);
            else type->SetSizeAndAlignment(elemType->typeSize * countElems, elemType->typeAlignment * 4);

            break;
        }

        // rules 5 and 7
        case spv::OpTypeMatrix:
        {
            ghfdsgfdsg;
            break;
        }

        case spv::OpTypeArray:
        case spv::OpTypeStruct:
        {
            ghfdsgfdsg;
            type->SetSizeAndAlignment(999, 999);
            break;
        }

        //===================================================================================
        //Scalars: rule 1
        case spv::OpTypeBool:
        {
            type->SetSizeAndAlignment(4, 4);
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
            type->SetSizeAndAlignment(nbByte, nbByte);
            break;
        }

        default:
            return error(string("Unknowns type OpCode: ") + spv::OpcodeString(type->GetOpCode()));
    }

    return true;
}

#endif