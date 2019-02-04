#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct ShaderB_TypeDeclaredInB
{
    int aInt;
};

layout(std140) uniform Globals
{
    ShaderB_TypeDeclaredInB ShaderMain_var1;
} Globals_var;

int ShaderA_function()
{
    return 1;
}

int ShaderMain_function()
{
    int res = ShaderA_function();
    res += Globals_var.ShaderMain_var1.aInt;
    res += 5;
    return res;
}

int main()
{
    return ShaderMain_function();
}

