#version 450

layout(std140) uniform CompA_globalCBuffer
{
    float varCA;
} CompA_globalCBuffer_var;

layout(std140) uniform compS8C0_CompA_globalCBuffer
{
    float varCA;
} compS8C0_CompA_globalCBuffer_var;

float compS8C0_CompA_Compute()
{
    return compS8C0_CompA_globalCBuffer_var.varCA;
}

float main()
{
    return compS8C0_CompA_Compute();
}

