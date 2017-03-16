#version 450

layout(std140) uniform globalCbuffer
{
    float varCA;
    float varCB;
} globalCbuffer_var;

float o0S5C0_CompA_Compute()
{
    return globalCbuffer_var.varCA;
}

float o1S5C1_CompB_Compute()
{
    return globalCbuffer_var.varCB;
}

float main()
{
    return o0S5C0_CompA_Compute() + o1S5C1_CompB_Compute();
}

