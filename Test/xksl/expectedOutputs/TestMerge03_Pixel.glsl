#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    float sbase1_id0;
};

layout(std140) uniform Globals
{
    int Base_Var1;
    float Base_Var2;
} Globals_var;

in float PS_IN_SBASE1;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0);
    _streams.sbase1_id0 = PS_IN_SBASE1;
    int i = int(((_streams.sbase1_id0 + float(Globals_var.Base_Var1)) + Globals_var.Base_Var2) + 2.0);
}

