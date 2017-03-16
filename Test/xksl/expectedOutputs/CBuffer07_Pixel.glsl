#version 450

layout(std140) uniform globalCbuffer
{
    float varA;
    float varC1;
    float varC1_1;
} globalCbuffer_var;

layout(std140) uniform PerDraw
{
    float varDraw1;
    float varDraw1_1;
} PerDraw_var;

float o0S5C0_ShaderComp_Compute()
{
    return globalCbuffer_var.varC1 + PerDraw_var.varDraw1;
}

float o1S5C1_ShaderComp_Compute()
{
    return globalCbuffer_var.varC1_1 + PerDraw_var.varDraw1_1;
}

float main()
{
    return (globalCbuffer_var.varA + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}

