#version 450

layout(std140) uniform Globals
{
    float o0S13C0_ShaderCompose_varCompose;
    float o0S13C0_ShaderComposeX_varComposeX;
} Globals_var;

float o0S13C0_ShaderComposeX_Compute()
{
    return Globals_var.o0S13C0_ShaderComposeX_varComposeX;
}

float o0S13C0_ShaderCompose_ComputeComp()
{
    return Globals_var.o0S13C0_ShaderCompose_varCompose + o0S13C0_ShaderComposeX_Compute();
}

void main()
{
    float f = o0S13C0_ShaderComposeX_Compute();
    f += o0S13C0_ShaderCompose_ComputeComp();
}

