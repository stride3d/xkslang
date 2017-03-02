#version 450

layout(std140) uniform o0S5C0_CompA_globalCBuffer
{
    float varCA;
} o0S5C0_CompA_globalCBuffer_var;

float o0S5C0_CompA_Compute()
{
    return o0S5C0_CompA_globalCBuffer_var.varCA;
}

float main()
{
    return o0S5C0_CompA_Compute();
}

