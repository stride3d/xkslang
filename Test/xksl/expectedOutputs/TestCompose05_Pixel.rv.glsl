#version 450

layout(std140) uniform CompA_globalCBuffer
{
    float varCA;
} CompA_globalCBuffer_var;

layout(std140) uniform CompB_globalCBuffer
{
    float varCB;
} CompB_globalCBuffer_var;

layout(std140) uniform compS8C0_CompA_globalCBuffer
{
    float varCA;
} compS8C0_CompA_globalCBuffer_var;

layout(std140) uniform compS8C0_CompB_globalCBuffer
{
    float varCB;
} compS8C0_CompB_globalCBuffer_var;

float compS8C0_CompB_Compute()
{
    return compS8C0_CompB_globalCBuffer_var.varCB;
}

float main()
{
    return compS8C0_CompB_Compute();
}

