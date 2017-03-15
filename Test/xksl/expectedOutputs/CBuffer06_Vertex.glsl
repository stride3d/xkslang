#version 450

layout(std140) uniform _globalCbuffer
{
    float var2;
    float var2_1;
} _globalCbuffer_var;

void main()
{
    float f44 = _globalCbuffer_var.var2_1;
}

