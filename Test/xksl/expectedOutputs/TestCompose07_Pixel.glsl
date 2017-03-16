#version 450

layout(std140) uniform globalCbuffer
{
    float varCA;
    float varCA_1;
    float varCB;
} globalCbuffer_var;

float o0S2C0_CompA_Compute()
{
    return globalCbuffer_var.varCA;
}

float o1S2C1_CompA_Compute()
{
    return globalCbuffer_var.varCA_1;
}

float o1S2C1_CompB_Compute()
{
    return globalCbuffer_var.varCB + o1S2C1_CompA_Compute();
}

float main()
{
    return o0S2C0_CompA_Compute() + o1S2C1_CompB_Compute();
}

