#version 450

layout(std140) uniform _globalCbuffer
{
    float varCB;
    float varCA;
} _globalCbuffer_var;

float o0S2C0_CompA_Compute()
{
    return _globalCbuffer_var.varCA;
}

float o0S2C0_CompB_Compute()
{
    return _globalCbuffer_var.varCB + o0S2C0_CompA_Compute();
}

float main()
{
    return o0S2C0_CompB_Compute();
}

