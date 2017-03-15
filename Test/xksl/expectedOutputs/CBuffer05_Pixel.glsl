#version 450

layout(std140) uniform ReferenceCBuffer
{
    int _aInt;
    float _aFloat;
    float _aFloatB;
    layout(row_major) mat4 _arrayA[3];
    float _varf;
    layout(row_major) mat3x4 _arrayB[4];
    layout(row_major) mat3x4 _af3x4;
    layout(row_major) imat4 _i4x4;
    layout(row_major) imat4 _i4x4A[2];
    layout(row_major) mat4 _arrayC[5];
    vec4 _arrayD[2];
    float _BleedingFactor;
    uint _aB320;
    uint _aB321;
    layout(row_major) mat4x2 _arrayCC[1];
    uint _aBA[2];
    vec4 _arrayAA4[4];
    vec2 _arrayAA2[4];
    vec4 _af02;
} ReferenceCBuffer_var;

layout(std140) uniform PerDraw
{
    int aInt;
    float aFloat;
    float aFloatB;
    layout(row_major) mat4 arrayA[3];
    float varf;
    layout(row_major) mat3x4 arrayB[4];
    layout(row_major) mat3x4 af3x4;
    layout(row_major) imat4 i4x4;
    layout(row_major) imat4 i4x4A[2];
    layout(row_major) mat4 arrayC[5];
    vec4 arrayD[2];
    float BleedingFactor;
    uint aB320;
    uint aB321;
    layout(row_major) mat4x2 arrayCC[1];
    uint aBA[2];
    vec4 arrayAA4[4];
    vec2 arrayAA2[4];
    vec4 af02;
} PerDraw_var;

void main()
{
    vec4 f4 = PerDraw_var.arrayD[1] + PerDraw_var.arrayAA4[0];
}

