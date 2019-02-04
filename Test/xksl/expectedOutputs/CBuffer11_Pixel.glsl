#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 Transform_WorldInverse;
    layout(row_major) mat4 Transform_WorldInverseTranspose;
    layout(row_major) mat4 Transform_WorldView;
    layout(row_major) mat4 Transform_WorldViewInverse;
    layout(row_major) mat4 Transform_WorldViewProjection;
    vec3 Transform_WorldScale;
    vec4 Transform_EyeMS;
} PerDraw_var;

layout(std140) uniform PerLight
{
    vec4 o0S2C0_ShaderComp_aFloat;
} PerLight_var;

vec4 o2S2C1_ShaderComp_Compute()
{
    return PerDraw_var.Transform_EyeMS + PerLight_var.o0S2C0_ShaderComp_aFloat;
}

vec4 o2S2C1_o1S2C0_ShaderComp_Compute()
{
    return PerDraw_var.Transform_EyeMS + PerLight_var.o0S2C0_ShaderComp_aFloat;
}

vec4 o2S2C1_ShaderComp2_Compute()
{
    return o2S2C1_ShaderComp_Compute() + o2S2C1_o1S2C0_ShaderComp_Compute();
}

void main()
{
    vec4 f = PerDraw_var.Transform_EyeMS + o2S2C1_ShaderComp2_Compute();
}

