#version 450

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
}

