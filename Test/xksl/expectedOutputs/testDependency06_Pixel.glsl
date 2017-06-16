#version 450

layout(std140) uniform Globals
{
    int ShaderA_varA;
} Globals_var;

int main()
{
    return Globals_var.ShaderA_varA;
}

