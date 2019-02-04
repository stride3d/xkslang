struct PS_STREAMS
{
    int s2_id0;
    int s1_id1;
};

static const PS_STREAMS _43 = { 0, 0 };

cbuffer Globals
{
    int o1S2C0_Comp_var1;
    int o1S2C0_o0S8C0_SubComp_var1;
    int o2S2C1_SubComp_var1;
};

static int PS_IN_S2;
static int PS_IN_S1;

struct SPIRV_Cross_Input
{
    int PS_IN_S1 : S1;
    int PS_IN_S2 : S2;
};

int o1S2C0_o0S8C0_SubComp_Compute(PS_STREAMS _streams)
{
    return o1S2C0_o0S8C0_SubComp_var1 + _streams.s2_id0;
}

int o1S2C0_Comp_Compute(PS_STREAMS _streams)
{
    return (o1S2C0_Comp_var1 + _streams.s1_id1) + o1S2C0_o0S8C0_SubComp_Compute(_streams);
}

int o2S2C1_SubComp_Compute(PS_STREAMS _streams)
{
    return o2S2C1_SubComp_var1 + _streams.s2_id0;
}

void frag_main()
{
    PS_STREAMS _streams = _43;
    _streams.s2_id0 = PS_IN_S2;
    _streams.s1_id1 = PS_IN_S1;
    int i = o1S2C0_Comp_Compute(_streams) + o2S2C1_SubComp_Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_S2 = stage_input.PS_IN_S2;
    PS_IN_S1 = stage_input.PS_IN_S1;
    frag_main();
}
