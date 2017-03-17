#version 450

layout(std140) uniform globalCbuffer
{
    float o0S2C0_CompA_varCA;
    float o0S2C0_CompB_varCB;
} globalCbuffer_var;

float o0S2C0_CompA_Compute()
{
    return globalCbuffer_var.o0S2C0_CompA_varCA;
}

float o0S2C0_CompB_Compute()
{
    return globalCbuffer_var.o0S2C0_CompB_varCB + o0S2C0_CompA_Compute();
}

float main()
{
    return o0S2C0_CompB_Compute();
}

