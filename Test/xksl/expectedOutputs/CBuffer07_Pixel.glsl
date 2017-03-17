#version 450

layout(std140) uniform globalCbuffer
{
    float ShaderA_varA;
    float o0S5C0_ShaderComp_varC1;
    float o1S5C1_ShaderComp_varC1;
} globalCbuffer_var;

layout(std140) uniform PerDraw
{
    float o0S5C0_ShaderComp_varDraw1;
    float o1S5C1_ShaderComp_varDraw1;
} PerDraw_var;

float o0S5C0_ShaderComp_Compute()
{
    return globalCbuffer_var.o0S5C0_ShaderComp_varC1 + PerDraw_var.o0S5C0_ShaderComp_varDraw1;
}

float o1S5C1_ShaderComp_Compute()
{
    return globalCbuffer_var.o1S5C1_ShaderComp_varC1 + PerDraw_var.o1S5C1_ShaderComp_varDraw1;
}

float main()
{
    return (globalCbuffer_var.ShaderA_varA + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}

