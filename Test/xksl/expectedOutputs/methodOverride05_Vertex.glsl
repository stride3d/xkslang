#version 450

layout(std140) uniform Globals
{
    float ShaderCompose_varCompose;
    float o1S13C1_ShaderComposeBY_varComposeBY;
} Globals_var;

float o1S13C1_ShaderComposeBY_Compute()
{
    return Globals_var.o1S13C1_ShaderComposeBY_varComposeBY;
}

float ShaderCompose_ComputeComp()
{
    return Globals_var.ShaderCompose_varCompose + o1S13C1_ShaderComposeBY_Compute();
}

void main()
{
    float f = o1S13C1_ShaderComposeBY_Compute();
    f += ShaderCompose_ComputeComp();
    f += ShaderCompose_ComputeComp();
}

