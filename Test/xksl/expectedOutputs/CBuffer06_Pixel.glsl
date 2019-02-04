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
    float f44 = Globals_var.ShaderPSMain_var2;
}

