#version 450

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 ShaderMain_BlendMatrixArray[5];
} PerDraw_var;

mat4 ShaderMain_GetBlendMatrix(int index)
{
    return PerDraw_var.ShaderMain_BlendMatrixArray[index];
}

void main()
{
    int param = 2;
    mat4 f44 = ShaderMain_GetBlendMatrix(param);
}

