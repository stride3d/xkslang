#version 450

layout(std140) uniform _globalCbuffer
{
    float varA;
    float varC1;
    float varC1_1;
} _globalCbuffer_var;

layout(std140) uniform PerDraw
{
    float varDraw1;
    float varDraw1_1;
} PerDraw_var;

float o0S5C0_ShaderComp_Compute()
{
    return _globalCbuffer_var.varC1_1 + PerDraw_var.varDraw1_1;
}

float o1S5C1_ShaderComp_Compute()
{
    return _globalCbuffer_var.varC1 + PerDraw_var.varDraw1;
}

float main()
{
    return (_globalCbuffer_var.varA + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}

