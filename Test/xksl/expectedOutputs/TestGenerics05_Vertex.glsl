#version 450

layout(std140) uniform globalCbuffer
{
    float ShaderBase_2_5__aVar;
} globalCbuffer_var;

float ShaderBase_2_5__compute()
{
    return (globalCbuffer_var.ShaderBase_2_5__aVar + 5.0) + 2.0;
}

float main()
{
    return ShaderBase_2_5__compute() + globalCbuffer_var.ShaderBase_2_5__aVar;
}

