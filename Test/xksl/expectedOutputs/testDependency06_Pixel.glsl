#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    int ShaderA_varA;
} Globals_var;

int main()
{
    return Globals_var.ShaderA_varA;
}

