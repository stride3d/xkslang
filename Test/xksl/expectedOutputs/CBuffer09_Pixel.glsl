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

float o1S2C1_ShaderComp_Compute()
{
    return PerDraw_var.varDraw1_1 + PerDraw_var.varDraw3_1;
}

float main()
{
    return o1S2C1_ShaderComp_Compute();
}

