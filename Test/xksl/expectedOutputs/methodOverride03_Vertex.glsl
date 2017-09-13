#version 450

layout(std140) uniform Globals
{
    float ShaderMainX_varMainX;
    float o0S13C0_ShaderCompose_varCompose;
} Globals_var;

float ShaderMainX_Compute()
{
    return Globals_var.ShaderMainX_varMainX;
}

float o0S13C0_ShaderCompose_ComputeComp()
{
    return Globals_var.o0S13C0_ShaderCompose_varCompose + ShaderMainX_Compute();
}

void main()
{
    float f = ShaderMainX_Compute();
    f += o0S13C0_ShaderCompose_ComputeComp();
}

