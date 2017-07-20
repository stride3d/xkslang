#version 450

layout(std140) uniform Globals
{
    float ShaderBase_id0_aVar;
} Globals_var;

float ShaderBase_2_5__compute()
{
    return (Globals_var.ShaderBase_id0_aVar + 5.0) + 2.0;
}

float main()
{
    return ShaderBase_2_5__compute() + Globals_var.ShaderBase_id0_aVar;
}

