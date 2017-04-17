#version 450

layout(std140) uniform globalCbuffer
{
    int ShaderA_varA;
} globalCbuffer_var;

int main()
{
    return globalCbuffer_var.ShaderA_varA;
}

