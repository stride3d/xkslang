#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderMain_ColorAberrations[5];
} Globals_var;

void main()
{
    float res = 0.0;
    for (int i = 0; i < 5; i++)
    {
        res += Globals_var.ShaderMain_ColorAberrations[i];
    }
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

