#version 450

layout(std140) uniform cS5C0_CompA_globalCBuffer
{
    float varCA;
} cS5C0_CompA_globalCBuffer_var;

float cS5C0_CompA_Compute()
{
    return cS5C0_CompA_globalCBuffer_var.varCA;
}

float main()
{
    return cS5C0_CompA_Compute();
}

