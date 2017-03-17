#version 450

layout(std140) uniform PerDraw
{
    int ShaderMain_aInt;
    float ShaderMain_aFloat;
    float ShaderMain_aFloatB;
    layout(row_major) mat4 ShaderMain_arrayA[3];
    float ShaderMain_varf;
    layout(row_major) mat3x4 ShaderMain_arrayB[4];
    layout(row_major) mat3x4 ShaderMain_af3x4;
    layout(row_major) imat4 ShaderMain_i4x4;
    layout(row_major) imat4 ShaderMain_i4x4A[2];
    layout(row_major) mat4 ShaderMain_arrayC[5];
    vec4 ShaderMain_arrayD[2];
    float ShaderMain_BleedingFactor;
    uint ShaderMain_aB320;
    uint ShaderMain_aB321;
    layout(row_major) mat4x2 ShaderMain_arrayCC[1];
    uint ShaderMain_aBA[2];
    vec4 ShaderMain_arrayAA4[4];
    vec2 ShaderMain_arrayAA2[4];
    vec4 ShaderMain_af02;
} PerDraw_var;

layout(std140) uniform ReferenceCBuffer
{
    int ShaderMain__aInt;
    float ShaderMain__aFloat;
    float ShaderMain__aFloatB;
    layout(row_major) mat4 ShaderMain__arrayA[3];
    float ShaderMain__varf;
    layout(row_major) mat3x4 ShaderMain__arrayB[4];
    layout(row_major) mat3x4 ShaderMain__af3x4;
    layout(row_major) imat4 ShaderMain__i4x4;
    layout(row_major) imat4 ShaderMain__i4x4A[2];
    layout(row_major) mat4 ShaderMain__arrayC[5];
    vec4 ShaderMain__arrayD[2];
    float ShaderMain__BleedingFactor;
    uint ShaderMain__aB320;
    uint ShaderMain__aB321;
    layout(row_major) mat4x2 ShaderMain__arrayCC[1];
    uint ShaderMain__aBA[2];
    vec4 ShaderMain__arrayAA4[4];
    vec2 ShaderMain__arrayAA2[4];
    vec4 ShaderMain__af02;
} ReferenceCBuffer_var;

void main()
{
    vec4 f4 = PerDraw_var.ShaderMain_arrayD[1] + PerDraw_var.ShaderMain_arrayAA4[0];
}

