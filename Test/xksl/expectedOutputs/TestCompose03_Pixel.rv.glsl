#version 450

layout(std140) uniform CompB_globalCBuffer
{
    float varCB;
} CompB_globalCBuffer_var;

layout(std140) uniform CompA_globalCBuffer
{
    float varCA;
} CompA_globalCBuffer_var;

layout(std140) uniform cS5C0_CompA_globalCBuffer
{
    float varCA;
} cS5C0_CompA_globalCBuffer_var;

layout(std140) uniform cS5C1_CompB_globalCBuffer
{
    float varCB;
} cS5C1_CompB_globalCBuffer_var;

float cS5C0_CompA_Compute()
{
    return cS5C0_CompA_globalCBuffer_var.varCA;
}

float cS5C1_CompB_Compute()
{
    return cS5C1_CompB_globalCBuffer_var.varCB;
}

float main()
{
    return cS5C0_CompA_Compute() + cS5C1_CompB_Compute();
}

