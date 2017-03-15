#version 450

layout(std140) uniform _globalCbuffer
{
    float varCB;
} _globalCbuffer_var;

float o0S2C0_CompB_Compute()
{
    return _globalCbuffer_var.varCB;
}

float main()
{
    return o0S2C0_CompB_Compute();
}

