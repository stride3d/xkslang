#version 450

layout(std140) uniform Globals
{
    float ShaderMain_aFloat;
} Globals_var;

void main()
{
    float f = Globals_var.ShaderMain_aFloat;
}

