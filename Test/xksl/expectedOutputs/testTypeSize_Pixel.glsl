#version 450

struct _44
{
    int i;
    vec4 f4;
    uint b;
};

struct _49
{
    mat4x3 aMat43;
    mat2 aMat22;
};

struct ShaderMain_StructOfStruct
{
    _49 s0;
    _44 s1;
};

struct ShaderMain_StructVector
{
    vec2 aFloat2;
    vec3 aFloat3;
    vec4 aFloat4;
};

struct ShaderMain_StructSimpleArray
{
    vec4 ColorArray[2];
    int IntArray[3];
    mat2x3 aMatArray[4];
};

struct ShaderMain_StructMatrixRowMajor
{
    mat4 aMat44_rm;
    mat3x4 aMat34_rm;
    mat2x3 aMat23_rm;
    mat4x3 aMat43_rm;
    mat4x2 aMat42_rm;
    mat3x2 aMat32_rm;
    mat3 aMat33_rm;
    mat2 aMat22_rm;
};

struct ShaderMain_StructMatrix
{
    mat4 aMat44;
    mat3x4 aMat34;
    mat2x3 aMat23;
    mat4x3 aMat43;
    mat4x2 aMat42;
    mat3x2 aMat32;
    mat3 aMat33;
    mat2 aMat22;
};

struct ShaderMain_StructBasicType
{
    float aHalf;
    uint aBool;
    int aInt;
    float aFloat;
    double aDouble;
};

layout(std140) uniform Globals
{
    layout(row_major) ShaderMain_StructBasicType ShaderMain_s1;
    layout(row_major) ShaderMain_StructVector ShaderMain_s2;
    layout(row_major) ShaderMain_StructMatrix ShaderMain_s3;
    layout(row_major) ShaderMain_StructMatrixRowMajor ShaderMain_s3_rm;
    layout(row_major) ShaderMain_StructSimpleArray ShaderMain_s4;
    layout(row_major) ShaderMain_StructOfStruct ShaderMain_s6;
} Globals_var;

void main()
{
    float f = 0.0;
    f += Globals_var.ShaderMain_s1.aFloat;
    f += Globals_var.ShaderMain_s2.aFloat2.x;
    f += Globals_var.ShaderMain_s3.aMat23[0].x;
    f += Globals_var.ShaderMain_s3_rm.aMat23_rm[0].x;
    f += Globals_var.ShaderMain_s4.ColorArray[0].x;
    f += Globals_var.ShaderMain_s6.s1.f4.x;
}

