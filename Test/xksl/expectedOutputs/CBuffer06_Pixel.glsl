#version 450

layout(std140) uniform Globals
{
    float ShaderPSMain_var2;
    float ShaderVSMain_var2;
} Globals_var;

void main()
{
    float f44 = Globals_var.ShaderPSMain_var2;
}

