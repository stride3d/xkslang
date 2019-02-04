#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderBase_aVar;
} Globals_var;

float ShaderBase_2_5__compute()
{
    return (Globals_var.ShaderBase_aVar + 5.0) + 2.0;
}

float main()
{
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
    return ShaderBase_2_5__compute() + Globals_var.ShaderBase_aVar;
}

