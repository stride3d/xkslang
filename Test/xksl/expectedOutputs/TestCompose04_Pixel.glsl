#version 450

layout(std140) uniform globalCbuffer
{
    float varCA;
} globalCbuffer_var;

float o0S5C0_CompA_Compute()
{
    return globalCbuffer_var.varCA;
}

float main()
{
    return o0S5C0_CompA_Compute();
}

