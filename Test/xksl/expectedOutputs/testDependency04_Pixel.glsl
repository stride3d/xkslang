#version 450

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

int ShaderE_staticFunction()
{
    return 2;
}

int ShaderMain_function()
{
    int res = ShaderA_function();
    res += Globals_var.ShaderMain_var1.aInt;
    res += 5;
    res += 4;
    res += ShaderE_staticFunction();
    return res;
}

int main()
{
    return ShaderMain_function();
}

