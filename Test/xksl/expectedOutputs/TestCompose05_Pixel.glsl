#version 450

layout(std140) uniform Globals
{
    float o0S2C0_CompB_varCB;
} Globals_var;

float o0S2C0_CompB_Compute()
{
    return Globals_var.o0S2C0_CompB_varCB;
}

float main()
{
    return o0S2C0_CompB_Compute();
}

