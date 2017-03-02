#version 450

layout(std140) uniform o0S2C0_CompA_globalCBuffer
{
    float varCA;
} o0S2C0_CompA_globalCBuffer_var;

layout(std140) uniform o1S2C1_CompA_globalCBuffer
{
    float varCA;
} o1S2C1_CompA_globalCBuffer_var;

layout(std140) uniform o1S2C1_CompB_globalCBuffer
{
    float varCB;
} o1S2C1_CompB_globalCBuffer_var;

float o0S2C0_CompA_Compute()
{
    return o0S2C0_CompA_globalCBuffer_var.varCA;
}

float o1S2C1_CompA_Compute()
{
    return o1S2C1_CompA_globalCBuffer_var.varCA;
}

float o1S2C1_CompB_Compute()
{
    return o1S2C1_CompB_globalCBuffer_var.varCB + o1S2C1_CompA_Compute();
}

float main()
{
    return o0S2C0_CompA_Compute() + o1S2C1_CompB_Compute();
}

