#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float o0S5C0_CompA_varCA;
    float o1S5C1_CompB_varCB;
} Globals_var;

float o0S5C0_CompA_Compute()
{
    return Globals_var.o0S5C0_CompA_varCA;
}

float o1S5C1_CompB_Compute()
{
    return Globals_var.o1S5C1_CompB_varCB;
}

float main()
{
    return o0S5C0_CompA_Compute() + o1S5C1_CompB_Compute();
}

