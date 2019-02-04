#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    float totoA_id0;
    float totoB_id1;
};

layout(std140) uniform Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
    float o2S55C0_ShaderComp_varC;
    float o3S55C1_ShaderComp_varC;
} Globals_var;

in float PS_IN_TOTOA;
out float PS_OUT_totoB;

float ShaderB_Compute()
{
    return 5.0;
}

float o2S55C0_ShaderComp_Compute()
{
    return Globals_var.o2S55C0_ShaderComp_varC;
}

float o3S55C1_ShaderComp_Compute()
{
    return Globals_var.o3S55C1_ShaderComp_varC;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0.0);
    _streams.totoA_id0 = PS_IN_TOTOA;
    float f = ShaderB_Compute();
    _streams.totoB_id1 = ((f + o2S55C0_ShaderComp_Compute()) + o3S55C1_ShaderComp_Compute()) + _streams.totoA_id0;
    PS_OUT_totoB = _streams.totoB_id1;
}

