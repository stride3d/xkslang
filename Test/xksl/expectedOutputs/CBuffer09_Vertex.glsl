#version 450

layout(std140) uniform PerDraw
{
    float varDraw1;
    float varDraw2;
    float varDraw3;
    float varDraw1_1;
    float varDraw2_1;
    float varDraw3_1;
} PerDraw_var;

float o0S2C0_ShaderComp_Compute()
{
    return PerDraw_var.varDraw1 + PerDraw_var.varDraw3;
}

float main()
{
    return o0S2C0_ShaderComp_Compute();
}

