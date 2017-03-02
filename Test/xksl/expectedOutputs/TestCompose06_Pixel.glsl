#version 450

layout(std140) uniform o0S2C0_CompA_globalCBuffer
{
    float varCA;
} o0S2C0_CompA_globalCBuffer_var;

layout(std140) uniform o0S2C0_CompB_globalCBuffer
{
    float varCB;
} o0S2C0_CompB_globalCBuffer_var;

float o0S2C0_CompA_Compute()
{
    return o0S2C0_CompA_globalCBuffer_var.varCA;
}

float o0S2C0_CompB_Compute()
{
    return o0S2C0_CompB_globalCBuffer_var.varCB + o0S2C0_CompA_Compute();
}

float main()
{
    return o0S2C0_CompB_Compute();
}

