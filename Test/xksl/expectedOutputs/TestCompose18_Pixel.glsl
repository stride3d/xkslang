#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
} Globals_var;

float ShaderA_Compute()
{
    return 5.0;
}

float o0S5C0_ShaderComp_Compute()
{
    return Globals_var.o0S5C0_ShaderComp_varC;
}

float o1S5C1_ShaderComp_Compute()
{
    return Globals_var.o1S5C1_ShaderComp_varC;
}

float main()
{
    float f = ShaderA_Compute();
    return (f + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}

