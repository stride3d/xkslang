#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    int s2_id0;
    int s1_id1;
};

layout(std140) uniform Globals
{
    int o1S2C0_Comp_var1;
    int o1S2C0_o0S8C0_SubComp_var1;
    int o2S2C1_SubComp_var1;
} Globals_var;

in int PS_IN_S2;
in int PS_IN_S1;

int o1S2C0_o0S8C0_SubComp_Compute(PS_STREAMS _streams)
{
    return Globals_var.o1S2C0_o0S8C0_SubComp_var1 + _streams.s2_id0;
}

int o1S2C0_Comp_Compute(PS_STREAMS _streams)
{
    return (Globals_var.o1S2C0_Comp_var1 + _streams.s1_id1) + o1S2C0_o0S8C0_SubComp_Compute(_streams);
}

int o2S2C1_SubComp_Compute(PS_STREAMS _streams)
{
    return Globals_var.o2S2C1_SubComp_var1 + _streams.s2_id0;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0, 0);
    _streams.s2_id0 = PS_IN_S2;
    _streams.s1_id1 = PS_IN_S1;
    int i = o1S2C0_Comp_Compute(_streams) + o2S2C1_SubComp_Compute(_streams);
}

