#version 450

layout(std140) uniform globalCbuffer
{
    float ShaderMain_aFloat;
} globalCbuffer_var;

void main()
{
    float f = globalCbuffer_var.ShaderMain_aFloat;
}

