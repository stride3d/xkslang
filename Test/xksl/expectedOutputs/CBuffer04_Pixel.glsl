#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform PerLighting
{
    float ShaderMain_BleedingFactor;
    float ShaderMain_MinVariance;
} PerLighting_var;

void main()
{
    float f = PerLighting_var.ShaderMain_MinVariance;
}

