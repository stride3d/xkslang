#version 450

layout(std140) uniform globalCbuffer
{
    float varCB;
} globalCbuffer_var;

float o0S2C0_CompB_Compute()
{
    return globalCbuffer_var.varCB;
}

float main()
{
    return o0S2C0_CompB_Compute();
}

