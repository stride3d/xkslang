#version 450

layout(std140) uniform cS2C0_CompA_globalCBuffer
{
    float varCA;
} cS2C0_CompA_globalCBuffer_var;

layout(std140) uniform cS2C0_CompB_globalCBuffer
{
    float varCB;
} cS2C0_CompB_globalCBuffer_var;

float cS2C0_CompB_Compute()
{
    return cS2C0_CompB_globalCBuffer_var.varCB;
}

float main()
{
    return cS2C0_CompB_Compute();
}

