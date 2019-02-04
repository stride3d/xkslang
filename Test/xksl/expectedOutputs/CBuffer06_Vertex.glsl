#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderPSMain_var2;
    float ShaderVSMain_var2;
} Globals_var;

void main()
{
    float f44 = Globals_var.ShaderVSMain_var2;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

