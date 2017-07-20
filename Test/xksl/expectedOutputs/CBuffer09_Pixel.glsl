#version 450

layout(std140) uniform PerToto
{
    float ShaderA_varToto;
    float o0S14C0_ShaderComp_varCompToto;
    float o1S14C1_ShaderComp_varCompToto;
} PerToto_var;

layout(std140) uniform Globals
{
    float ShaderA_varG1;
    float ShaderA_varG2;
    float o0S14C0_ShaderComp_varG1;
    float o0S14C0_ShaderComp_varG2;
    float o1S14C1_ShaderComp_varG1;
} Globals_var;

layout(std140) uniform PerDraw
{
    float ShaderA_varDrawA;
    float ShaderA_varDraw3;
    float o0S14C0_ShaderComp_varDraw1;
    float o0S14C0_ShaderComp_varDraw2;
    float o0S14C0_ShaderComp_varDraw3;
    float o1S14C1_ShaderComp_varDraw3;
} PerDraw_var;

float o1S14C1_ShaderComp_Compute()
{
    return (((PerDraw_var.o0S14C0_ShaderComp_varDraw1 + PerDraw_var.o1S14C1_ShaderComp_varDraw3) + Globals_var.o1S14C1_ShaderComp_varG1) + Globals_var.o0S14C0_ShaderComp_varG2) + PerToto_var.o1S14C1_ShaderComp_varCompToto;
}

float main()
{
    return (PerDraw_var.ShaderA_varDrawA + o1S14C1_ShaderComp_Compute()) + Globals_var.ShaderA_varG2;
}

