#version 450

layout(std140) uniform globalCbuffer
{
    float var1;
    float var2;
    float var5;
    float var1_1;
    float var8;
} globalCbuffer_var;

void main()
{
    float f = (globalCbuffer_var.var5 + globalCbuffer_var.var2) + globalCbuffer_var.var1;
}

