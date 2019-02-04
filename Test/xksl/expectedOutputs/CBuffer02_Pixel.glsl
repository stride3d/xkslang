#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderMain_var2;
    float ShaderMain_var4[4];
    vec4 ShaderMain_var7;
} Globals_var;

layout(std140) uniform CBufferPixelStage
{
    float ShaderMain_MinVariance;
} CBufferPixelStage_var;

void main()
{
    float f = CBufferPixelStage_var.ShaderMain_MinVariance + Globals_var.ShaderMain_var7.x;
}

