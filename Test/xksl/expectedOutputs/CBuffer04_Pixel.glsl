#version 450

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 BlendMatrixArray[5];
} PerDraw_var;

layout(std140) uniform PerLighting
{
    float BleedingFactor;
    float MinVariance;
} PerLighting_var;

void main()
{
    float f = PerLighting_var.MinVariance;
}

