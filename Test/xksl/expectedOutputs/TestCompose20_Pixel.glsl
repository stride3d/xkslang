#version 450

layout(std140) uniform Globals
{
    float o0S29C0_ShaderComp_varC;
    float o1S16C0_ShaderComp_varC;
} Globals_var;

float o0S29C0_ShaderComp_Compute()
{
    return Globals_var.o0S29C0_ShaderComp_varC;
}

float ShaderA_Compute()
{
    return 1.0 + o0S29C0_ShaderComp_Compute();
}

float o1S16C0_ShaderComp_Compute()
{
    return Globals_var.o1S16C0_ShaderComp_varC;
}

float ShaderB_Compute()
{
    return 2.0 + o1S16C0_ShaderComp_Compute();
}

float ShaderMain_Compute()
{
    return ShaderA_Compute() + ShaderB_Compute();
}

void main()
{
    float f = ShaderMain_Compute();
}

