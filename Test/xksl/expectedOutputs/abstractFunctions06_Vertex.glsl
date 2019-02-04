#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderMain_varMain;
} Globals_var;

float ShaderMain_Compute()
{
    return Globals_var.ShaderMain_varMain;
}

void main()
{
    float f = ShaderMain_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

