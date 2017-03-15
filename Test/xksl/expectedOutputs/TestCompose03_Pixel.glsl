#version 450

layout(std140) uniform _globalCbuffer
{
    float varCB;
    float varCA;
} _globalCbuffer_var;

float o0S5C0_CompA_Compute()
{
    return _globalCbuffer_var.varCA;
}

float o1S5C1_CompB_Compute()
{
    return _globalCbuffer_var.varCB;
}

float main()
{
    return o0S5C0_CompA_Compute() + o1S5C1_CompB_Compute();
}

