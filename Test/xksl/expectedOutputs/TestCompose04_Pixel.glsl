#version 450

layout(std140) uniform Globals
{
    float o0S5C0_CompA_varCA;
} Globals_var;

float o0S5C0_CompA_Compute()
{
    return Globals_var.o0S5C0_CompA_varCA;
}

float main()
{
    return o0S5C0_CompA_Compute();
}

