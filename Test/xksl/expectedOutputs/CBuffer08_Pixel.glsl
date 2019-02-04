#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderPSMain_var5;
    float ShaderPSMain_var1;
    float ShaderPSMain_var2;
    float ShaderVSMain_var1;
    float ShaderVSMain_var8;
} Globals_var;

void main()
{
    float f = (Globals_var.ShaderPSMain_var5 + Globals_var.ShaderPSMain_var2) + Globals_var.ShaderPSMain_var1;
}

