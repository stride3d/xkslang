#version 450

layout(std140) uniform Globals
{
    float ShaderMain_varMain;
    float o0S5C0_ShaderCompose_varCompose;
} Globals_var;

float ShaderMain_Compute()
{
    return Globals_var.ShaderMain_varMain;
}

float o0S5C0_ShaderCompose_ComputeComp()
{
    return Globals_var.o0S5C0_ShaderCompose_varCompose + ShaderMain_Compute();
}

void main()
{
    float f = ShaderMain_Compute();
    f += o0S5C0_ShaderCompose_ComputeComp();
}

