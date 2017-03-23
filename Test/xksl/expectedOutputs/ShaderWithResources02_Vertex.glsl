#version 450

layout(std140) uniform ResourceGroup
{
    float ShaderMain_aFloat;
} ResourceGroup_var;

void main()
{
    float f = ResourceGroup_var.ShaderMain_aFloat;
}

