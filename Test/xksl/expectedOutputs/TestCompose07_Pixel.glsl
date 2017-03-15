#version 450

layout(std140) uniform _globalCbuffer
{
    float varCB;
    float varCA;
    float varCA_1;
} _globalCbuffer_var;

float o0S2C0_CompA_Compute()
{
    return _globalCbuffer_var.varCA;
}

float o1S2C1_CompA_Compute()
{
    return _globalCbuffer_var.varCA_1;
}

float o1S2C1_CompB_Compute()
{
    return _globalCbuffer_var.varCB + o1S2C1_CompA_Compute();
}

float main()
{
    return o0S2C0_CompA_Compute() + o1S2C1_CompB_Compute();
}

