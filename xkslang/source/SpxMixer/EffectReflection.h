//
// Copyright (C) 

#ifndef XKSLANG_XKSL_EFFECT_REFLECTION_H__
#define XKSLANG_XKSL_EFFECT_REFLECTION_H__

#include <string>
#include <vector>

#include "../Common/define.h"

namespace xkslang
{

enum class EffectParameterReflectionType
{
    Undefined = -1,

    /// <summary>
    /// <dd> <p>The variable is a void reference.</p> </dd>
    /// </summary>
    Void = 0,

    /// <summary>
    /// <dd> <p>The variable is a boolean.</p> </dd>
    /// </summary>
    Bool = 1,

    /// <summary>
    /// <dd> <p>The variable is an integer.</p> </dd>
    /// </summary>
    Int = 2,

    /// <summary>
    /// <dd> <p>The variable is a floating-point number.</p> </dd>
    /// </summary>
    Float = 3,

    /// <summary>
    /// <dd> <p>The variable is a string.</p> </dd>
    /// </summary>
    String = 4,

    /// <summary>
    /// <dd> <p>The variable is a texture.</p> </dd>
    /// </summary>
    Texture = 5,

    /// <summary>
    /// <dd> <p>The variable is a 1D texture.</p> </dd>
    /// </summary>
    Texture1D = 6,

    /// <summary>
    /// <dd> <p>The variable is a 2D texture.</p> </dd>
    /// </summary>
    Texture2D = 7,

    /// <summary>
    /// <dd> <p>The variable is a 3D texture.</p> </dd>
    /// </summary>
    Texture3D = 8,

    /// <summary>
    /// <dd> <p>The variable is a texture cube.</p> </dd>
    /// </summary>
    TextureCube = 9,

    /// <summary>
    /// <dd> <p>The variable is a sampler.</p> </dd>
    /// </summary>
    Sampler = 10,

    /// <summary>
    /// <dd> <p>The variable is a sampler.</p> </dd>
    /// </summary>
    Sampler1D = 11,

    /// <summary>
    /// <dd> <p>The variable is a sampler.</p> </dd>
    /// </summary>
    Sampler2D = 12,

    /// <summary>
    /// <dd> <p>The variable is a sampler.</p> </dd>
    /// </summary>
    Sampler3D = 13,

    /// <summary>
    /// <dd> <p>The variable is a sampler.</p> </dd>
    /// </summary>
    SamplerCube = 14,

    /// <summary>
    /// <dd> <p>The variable is an unsigned integer.</p> </dd>
    /// </summary>
    UInt = 19,

    /// <summary>
    /// <dd> <p>The variable is an 8-bit unsigned integer.</p> </dd>
    /// </summary>
    UInt8 = 20,

    /// <summary>
    /// <dd> <p>The variable is a buffer.</p> </dd>
    /// </summary>
    Buffer = 25,

    /// <summary>
    /// <dd> <p>The variable is a constant buffer.</p> </dd>
    /// </summary>
    ConstantBuffer = 26,

    /// <summary>
    /// <dd> <p>The variable is a texture buffer.</p> </dd>
    /// </summary>
    TextureBuffer = 27,

    /// <summary>
    /// <dd> <p>The variable is a 1D-texture array.</p> </dd>
    /// </summary>
    Texture1DArray = 28,

    /// <summary>
    /// <dd> <p>The variable is a 2D-texture array.</p> </dd>
    /// </summary>
    Texture2DArray = 29,

    /// <summary>
    /// <dd> <p>The variable is a 2D-multisampled texture.</p> </dd>
    /// </summary>
    Texture2DMultisampled = 32,

    /// <summary>
    /// <dd> <p>The variable is a 2D-multisampled-texture array.</p> </dd>
    /// </summary>
    Texture2DMultisampledArray = 33,

    /// <summary>
    /// <dd> <p>The variable is a texture-cube array.</p> </dd>
    /// </summary>
    TextureCubeArray = 34,

    /// <summary>
    /// <dd> <p>The variable is a double precision (64-bit) floating-point number.</p> </dd>
    /// </summary>
    Double = 39,

    /// <summary>
    /// <dd> <p>The variable is a 1D read-and-write texture.</p> </dd>
    /// </summary>
    RWTexture1D = 40,

    /// <summary>
    /// <dd> <p>The variable is an array of 1D read-and-write textures.</p> </dd>
    /// </summary>
    RWTexture1DArray = 41,

    /// <summary>
    /// <dd> <p>The variable is a 2D read-and-write texture.</p> </dd>
    /// </summary>
    RWTexture2D = 42,

    /// <summary>
    /// <dd> <p>The variable is an array of 2D read-and-write textures.</p> </dd>
    /// </summary>
    RWTexture2DArray = 43,

    /// <summary>
    /// <dd> <p>The variable is a 3D read-and-write texture.</p> </dd>
    /// </summary>
    RWTexture3D = 44,

    /// <summary>
    /// <dd> <p>The variable is a read-and-write buffer.</p> </dd>
    /// </summary>
    RWBuffer = 45,

    /// <summary>
    /// <dd> <p>The variable is a byte-address buffer.</p> </dd>
    /// </summary>
    ByteAddressBuffer = 46,

    /// <summary>
    /// <dd> <p>The variable is a read-and-write byte-address buffer.</p> </dd>
    /// </summary>
    RWByteAddressBuffer = 47,

    /// <summary>
    /// <dd> <p>The variable is a structured buffer. </p> <p>For more information about structured buffer, see the <strong>Remarks</strong> section.</p> </dd>
    /// </summary>
    StructuredBuffer = 48,

    /// <summary>
    /// <dd> <p>The variable is a read-and-write structured buffer.</p> </dd>
    /// </summary>
    RWStructuredBuffer = 49,

    /// <summary>
    /// <dd> <p>The variable is an append structured buffer.</p> </dd>
    /// </summary>
    AppendStructuredBuffer = 50,

    /// <summary>
    /// <dd> <p>The variable is a consume structured buffer.</p> </dd>
    /// </summary>
    ConsumeStructuredBuffer = 51,
};

enum class EffectParameterReflectionClass
{
    Undefined = -1,

    /// <summary>
    /// <dd> <p>The shader variable is a scalar.</p> </dd>
    /// </summary>
    Scalar = 0,

    /// <summary>
    /// <dd> <p>The shader variable is a vector.</p> </dd>
    /// </summary>
    Vector = 1,

    /// <summary>
    /// <dd> <p>The shader variable is a row-major matrix.</p> </dd>
    /// </summary>
    MatrixRows = 2,

    /// <summary>
    /// <dd> <p>The shader variable is a column-major matrix.</p> </dd>
    /// </summary>
    MatrixColumns = 3,

    /// <summary>
    /// <dd> <p>The shader variable is an object.</p> </dd>
    /// </summary>
    Object = 4,

    /// <summary>
    /// <dd> <p>The shader variable is a structure.</p> </dd>
    /// </summary>
    Struct = 5,

    /// <summary>
    /// <dd> <p>The shader variable is a class.</p> </dd>
    /// </summary>
    InterfaceClass = 6,

    /// <summary>
    /// <dd> <p>The shader variable is an interface.</p> </dd>
    /// </summary>
    InterfacePointer = 7,

    /// <summary>
    /// A sampler state object.
    /// </summary>
    Sampler = 8,

    /// <summary>
    /// A shader resource view.
    /// </summary>
    ShaderResourceView = 9,

    /// <summary>
    /// A constant buffer
    /// </summary>
    ConstantBuffer = 10,

    /// <summary>
    /// A constant buffer
    /// </summary>
    TextureBuffer = 11,

    /// <summary>
    /// An unordered access view
    /// </summary>
    UnorderedAccessView = 12,

    /// <summary>
    /// <dd> <p>The shader variable is a vector.</p> </dd>
    /// </summary>
    Color = 13,
};

class TypeMemberReflectionDescription;
class TypeReflectionDescription
{
public:
    TypeReflectionDescription() : Class(EffectParameterReflectionClass::Undefined), Type(EffectParameterReflectionType::Undefined),
        RowCount(0), ColumnCount(0), ArrayElements(0), Size(0), Alignment(0), ArrayStride(0), MatrixStride(0), Members(nullptr), CountMembers(0){}

    TypeReflectionDescription(const TypeReflectionDescription& t) : Class(t.Class), Type(t.Type),
        RowCount(t.RowCount), ColumnCount(t.ColumnCount), ArrayElements(t.ArrayElements), Size(t.Size), Alignment(t.Alignment),
        ArrayStride(t.ArrayStride), MatrixStride(t.MatrixStride), Members(nullptr), CountMembers(0)
    {}

    virtual ~TypeReflectionDescription();

    bool isValid() {return Class != EffectParameterReflectionClass::Undefined;}

    bool isScalarType() {return Class == EffectParameterReflectionClass::Scalar;}

    void Set(EffectParameterReflectionClass c, EffectParameterReflectionType t, int countRow, int countColumn, int size, int alignment, int arrayStride, int matrixStride, int arrayElements)
    {
        this->Class = c;
        this->Type = t;
        this->RowCount = countRow;
        this->ColumnCount = countColumn;
        this->Size = size;
        this->Alignment = alignment;
        this->ArrayStride = arrayStride;
        this->MatrixStride = matrixStride;
        this->ArrayElements = arrayElements;
    }

    std::string Print();

public:
    /// <summary>
    /// The <see cref="EffectParameterReflectionClass"/> of this parameter.
    /// </summary>
    EffectParameterReflectionClass Class;

    /// <summary>
    /// The <see cref="EffectParameterReflectionType"/> of this parameter.
    /// </summary>
    EffectParameterReflectionType Type;

    /// <summary>
    /// Number of rows for this element.
    /// </summary>
    int RowCount;

    /// <summary>
    /// Number of columns for this element.
    /// </summary>
    int ColumnCount;

    /// <summary>
    /// Number of elements for arrays (0 if not an array).
    /// </summary>
    int ArrayElements;

    /// <summary>
    /// Size of this element (non-aligned).
    /// </summary>
    int Size;

    /// <summary>
    /// Alignment of this element.
    /// </summary>
    int Alignment;

    /// <summary>
    /// Stride of this element (for Arrays)
    /// </summary>
    int ArrayStride;

    /// <summary>
    /// Stride of this element (for Matrices)
    /// </summary>
    int MatrixStride;

    /// <summary>
    /// Name of this structure type.
    /// </summary>
    std::string Name;

    /// <summary>
    /// Members in the structure.
    /// </summary>
    TypeMemberReflectionDescription* Members;
    int CountMembers;
};

class TypeMemberReflectionDescription
{
public:
    TypeMemberReflectionDescription() : Offset(0) {}

public:
    /// <summary>
    /// The name of this member.
    /// </summary>
    std::string Name;

    /// <summary>
    /// Offset in bytes into the parent structure (0 if not a structure member).
    /// </summary>
    int Offset;

    /// <summary>
    /// The type of this member.
    /// </summary>
    TypeReflectionDescription Type;
};

class ConstantBufferMemberReflectionDescription
{
public:
    std::string KeyName;

    int Offset;
    TypeReflectionDescription ReflectionType;

    ConstantBufferMemberReflectionDescription(){}

public:
    std::string Print();
};

class ConstantBufferReflectionDescription
{
public:
    int Size;
    std::string CbufferName;
    std::vector<ConstantBufferMemberReflectionDescription> Members;

    ConstantBufferReflectionDescription() : Size(0){}
};

class EffectResourceBindingDescription
{
public:
    ShadingStageEnum Stage;
    std::string KeyName;
    EffectParameterReflectionClass Class;
    EffectParameterReflectionType Type;

public:
    EffectResourceBindingDescription(){}
    EffectResourceBindingDescription(ShadingStageEnum stage, std::string keyName, EffectParameterReflectionClass c, EffectParameterReflectionType t)
        : Stage(stage), KeyName(keyName), Class(c), Type(t) {}
};

class EffectReflection
{
public:
    std::vector<ConstantBufferReflectionDescription> ConstantBuffers;
    std::vector<EffectResourceBindingDescription> ResourceBindings;

public:
    EffectReflection() {}

    void Clear();

    static std::string GetEffectParameterReflectionClassLabel(EffectParameterReflectionClass parameterClass);
    static std::string GetEffectParameterReflectionTypeLabel(EffectParameterReflectionType parameterType);
    std::string Print();
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_EFFECT_REFLECTION_H__
