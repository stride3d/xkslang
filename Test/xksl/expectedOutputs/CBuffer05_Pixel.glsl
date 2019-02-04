#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

void main()
{
    vec4 f4 = PerDraw_var.ShaderMain_arrayD[1] + PerDraw_var.ShaderMain_arrayAA4[0];
}

