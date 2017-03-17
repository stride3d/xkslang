#version 450

layout(std140) uniform globalCbuffer
{
    float ShaderPSMain_var2;
    float ShaderVSMain_var2;
} globalCbuffer_var;

void main()
{
    float f44 = globalCbuffer_var.ShaderVSMain_var2;
}

