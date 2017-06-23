struct _44
{
    int i;
    float4 f4;
    uint b;
};

struct _49
{
    float4x3 aMat43;
    float2x2 aMat22;
};

struct ShaderMain_StructOfStruct
{
    _49 s0;
    _44 s1;
};

struct ShaderMain_StructVector
{
    float2 aFloat2;
    float3 aFloat3;
    float4 aFloat4;
};

struct ShaderMain_StructSimpleArray
{
    float4 ColorArray[2];
    int IntArray[3];
    float2x3 aMatArray[4];
};

struct ShaderMain_StructMatrixRowMajor
{
    float4x4 aMat44_rm;
    float3x4 aMat34_rm;
    float2x3 aMat23_rm;
    float4x3 aMat43_rm;
    float4x2 aMat42_rm;
    float3x2 aMat32_rm;
    float3x3 aMat33_rm;
    float2x2 aMat22_rm;
};

struct ShaderMain_StructMatrix
{
    float4x4 aMat44;
    float3x4 aMat34;
    float2x3 aMat23;
    float4x3 aMat43;
    float4x2 aMat42;
    float3x2 aMat32;
    float3x3 aMat33;
    float2x2 aMat22;
};

struct ShaderMain_StructBasicType
{
    float aHalf;
    uint aBool;
    int aInt;
    float aFloat;
    double aDouble;
};

cbuffer Globals
{
    ShaderMain_StructBasicType ShaderMain_s1;
    ShaderMain_StructVector ShaderMain_s2;
    ShaderMain_StructMatrix ShaderMain_s3;
    ShaderMain_StructMatrixRowMajor ShaderMain_s3_rm;
    ShaderMain_StructSimpleArray ShaderMain_s4;
    ShaderMain_StructOfStruct ShaderMain_s6;
};

void frag_main()
{
    float f = 0.0f;
    f += ShaderMain_s1.aFloat;
    f += ShaderMain_s2.aFloat2.x;
    f += ShaderMain_s3.aMat23[0].x;
    f += ShaderMain_s3_rm.aMat23_rm[0].x;
    f += ShaderMain_s4.ColorArray[0].x;
    f += ShaderMain_s6.s1.f4.x;
}

void main()
{
    frag_main();
}
