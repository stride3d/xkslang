#version 450

layout(std140) uniform globalCbuffer
{
    float var2;
    float var2_1;
} globalCbuffer_var;

void main()
{
    float f44 = globalCbuffer_var.var2_1;
}

