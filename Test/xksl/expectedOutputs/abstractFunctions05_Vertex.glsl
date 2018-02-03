#version 450

layout(std140) uniform Globals
{
    float ShaderMainB_varMaterialSurface;
} Globals_var;

float ShaderMainB_Compute()
{
    return Globals_var.ShaderMainB_varMaterialSurface;
}

void main()
{
    float f = ShaderMainB_Compute();
}

