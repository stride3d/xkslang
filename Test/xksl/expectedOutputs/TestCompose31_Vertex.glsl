#version 450

layout(std140) uniform cbComposition
{
    float o0S2C0_ShaderComposition_var09;
} cbComposition_var;

float o0S2C0_ShaderComposition_Compute()
{
    return cbComposition_var.o0S2C0_ShaderComposition_var09;
}

void main()
{
    float f = 0.0;
    f += o0S2C0_ShaderComposition_Compute();
}

