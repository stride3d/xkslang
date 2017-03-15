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

mat4 ShaderMain_GetBlendMatrix(int index)
{
    return PerDraw_var.BlendMatrixArray[index];
}

void main()
{
    int param = 2;
    mat4 f44 = ShaderMain_GetBlendMatrix(param);
}

