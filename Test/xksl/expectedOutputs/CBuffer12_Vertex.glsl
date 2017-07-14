#version 450

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
    vec4 ShaderComp_aFloat;
} PerLight_var;

vec4 o0S2C0_ShaderComp_Compute()
{
    return PerDraw_var.Transform_EyeMS + PerLight_var.ShaderComp_aFloat;
}

void main()
{
    vec4 f = PerDraw_var.Transform_EyeMS + o0S2C0_ShaderComp_Compute();
}

