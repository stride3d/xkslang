#version 450

layout(std140) uniform cbUnstageComposition
{
    float o0S2C0_ShaderComposition_var09;
} cbUnstageComposition_var;

layout(std140) uniform cbStageComposition
{
    float o0S2C0_ShaderComposition_var10;
} cbStageComposition_var;

float o0S2C0_ShaderComposition_Compute()
{
    return cbUnstageComposition_var.o0S2C0_ShaderComposition_var09 + cbStageComposition_var.o0S2C0_ShaderComposition_var10;
}

void main()
{
    float f = 0.0;
    f += o0S2C0_ShaderComposition_Compute();
}

