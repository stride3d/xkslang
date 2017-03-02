#version 450

layout(std140) uniform o0S5C0_CompA_globalCBuffer
{
    float varCA;
} o0S5C0_CompA_globalCBuffer_var;

layout(std140) uniform o1S5C1_CompB_globalCBuffer
{
    float varCB;
} o1S5C1_CompB_globalCBuffer_var;

float o0S5C0_CompA_Compute()
{
    return o0S5C0_CompA_globalCBuffer_var.varCA;
}

float o1S5C1_CompB_Compute()
{
    return o1S5C1_CompB_globalCBuffer_var.varCB;
}

float main()
{
    return o0S5C0_CompA_Compute() + o1S5C1_CompB_Compute();
}

