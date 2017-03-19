#version 450

layout(std140) uniform PerLighting
{
    float ShaderMain_BleedingFactor;
    float ShaderMain_MinVariance;
} PerLighting_var;

void main()
{
    float f = PerLighting_var.ShaderMain_MinVariance;
}

