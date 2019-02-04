#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float o0S2C0_CompA_varCA;
    float o0S2C0_CompB_varCB;
} Globals_var;

float o0S2C0_CompA_Compute()
{
    return Globals_var.o0S2C0_CompA_varCA;
}

float o0S2C0_CompB_Compute()
{
    return Globals_var.o0S2C0_CompB_varCB + o0S2C0_CompA_Compute();
}

float main()
{
    return o0S2C0_CompB_Compute();
}

