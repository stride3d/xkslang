#version 450

layout(std140) uniform _globalCbuffer
{
    float varCA;
} _globalCbuffer_var;

float o0S5C0_CompA_Compute()
{
    return _globalCbuffer_var.varCA;
}

float main()
{
    return o0S5C0_CompA_Compute();
}

