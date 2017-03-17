#version 450

layout(std140) uniform globalCbuffer
{
    float o0S5C0_CompA_varCA;
} globalCbuffer_var;

float o0S5C0_CompA_Compute()
{
    return globalCbuffer_var.o0S5C0_CompA_varCA;
}

float main()
{
    return o0S5C0_CompA_Compute();
}

