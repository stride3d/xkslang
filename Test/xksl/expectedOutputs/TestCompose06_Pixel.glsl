#version 450

layout(std140) uniform globalCbuffer
{
    float varCA;
    float varCB;
} globalCbuffer_var;

float o0S2C0_CompA_Compute()
{
    return globalCbuffer_var.varCA;
}

float o0S2C0_CompB_Compute()
{
    return globalCbuffer_var.varCB + o0S2C0_CompA_Compute();
}

float main()
{
    return o0S2C0_CompB_Compute();
}

