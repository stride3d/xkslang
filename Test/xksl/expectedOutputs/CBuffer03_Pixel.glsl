#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform CBufferToto
{
    float ShaderMain_BleedingFactor;
    float ShaderMain_MinVariance;
} CBufferToto_var;

void main()
{
    float f = CBufferToto_var.ShaderMain_MinVariance;
}

