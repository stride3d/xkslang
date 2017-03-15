#version 450

layout(std140) uniform PerLighting
{
    float BleedingFactor;
    float MinVariance;
} PerLighting_var;

void main()
{
    float f = PerLighting_var.MinVariance;
}

