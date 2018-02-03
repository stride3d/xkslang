#version 450

layout(std140) uniform Globals
{
    float ShaderMainB_varMainB;
} Globals_var;

float ShaderMainB_stageCall()
{
    return Globals_var.ShaderMainB_varMainB;
}

void main()
{
    float f = ShaderMainB_stageCall();
}

