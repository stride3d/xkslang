#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float DirectLightGroupArray_varf;
} Globals_var;

float o0S2C0_IMaterialSurface_Compute()
{
    return Globals_var.DirectLightGroupArray_varf;
}

void main()
{
    float f = Globals_var.DirectLightGroupArray_varf;
    f += o0S2C0_IMaterialSurface_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

