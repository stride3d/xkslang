#version 450

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 ShaderMain_BlendMatrixArray[5];
} PerDraw_var;

layout(std140) uniform PerLighting
{
    float ShaderMain_BleedingFactor;
    float ShaderMain_MinVariance;
} PerLighting_var;

void main()
{
    float f = PerLighting_var.ShaderMain_MinVariance;
}

