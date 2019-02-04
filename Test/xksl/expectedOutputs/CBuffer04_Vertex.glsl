#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

