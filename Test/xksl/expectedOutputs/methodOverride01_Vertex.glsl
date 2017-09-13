#version 450

layout(std140) uniform Globals
{
    float ShaderBase_varBase;
    float o0S2C0_ShaderCompose_varCompose;
    float o0S2C0_ShaderBase_varBase;
} Globals_var;

float ShaderBase_Compute()
{
    return Globals_var.ShaderBase_varBase;
}

float o0S2C0_ShaderBase_Compute()
{
    return Globals_var.o0S2C0_ShaderBase_varBase;
}

float o0S2C0_ShaderCompose_ComputeComp()
{
    return Globals_var.o0S2C0_ShaderCompose_varCompose + o0S2C0_ShaderBase_Compute();
}

void main()
{
    float f = ShaderBase_Compute();
    f += o0S2C0_ShaderCompose_ComputeComp();
}

