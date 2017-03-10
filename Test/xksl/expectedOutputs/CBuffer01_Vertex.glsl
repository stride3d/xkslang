#version 450

layout(std140) uniform ShaderMain_PerLighting_0
{
    float BleedingFactor;
    float MinVariance;
} ShaderMain_PerLighting_0_var;

void main()
{
    float f = ShaderMain_PerLighting_0_var.BleedingFactor;
}

