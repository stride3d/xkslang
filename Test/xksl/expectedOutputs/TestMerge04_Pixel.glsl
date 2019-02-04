#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    float sbase1_id0;
    int sa1_id1;
};

layout(std140) uniform Globals
{
    int Base_Var1;
} Globals_var;

layout(std140) uniform C1
{
    int shaderA_Var1;
} C1_var;

in float PS_IN_SBASE1;
in int PS_IN_SA1;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0);
    _streams.sbase1_id0 = PS_IN_SBASE1;
    _streams.sa1_id1 = PS_IN_SA1;
    int i = int(((((2.0 + _streams.sbase1_id0) + float(C1_var.shaderA_Var1)) + float(Globals_var.Base_Var1)) + 5.0) + float(_streams.sa1_id1));
}

