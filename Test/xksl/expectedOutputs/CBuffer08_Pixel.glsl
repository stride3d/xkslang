#version 450

layout(std140) uniform Globals
{
    float ShaderPSMain_var5;
    float ShaderPSMain_var1;
    float ShaderPSMain_var2;
    float ShaderVSMain_var1;
    float ShaderVSMain_var8;
} Globals_var;

void main()
{
    float f = (Globals_var.ShaderPSMain_var5 + Globals_var.ShaderPSMain_var2) + Globals_var.ShaderPSMain_var1;
}

