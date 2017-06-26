struct PS_STREAMS
{
    int s1_id0;
    int s1_id1;
    int s1_id2;
};

cbuffer Globals
{
    int o1S2C0_Comp_var1;
    int o1S2C0_o0S8C0_SubComp_var1;
    int o2S2C1_SubComp_var1;
};

static int PS_IN_s1;
static int PS_IN_s1_1;
static int PS_IN_s1_2;

struct SPIRV_Cross_Input
{
    int PS_IN_s1 : TEXCOORD0;
    int PS_IN_s1_1 : TEXCOORD1;
    int PS_IN_s1_2 : TEXCOORD2;
};

int o1S2C0_o0S8C0_SubComp_Compute(PS_STREAMS _streams)
{
    return o1S2C0_o0S8C0_SubComp_var1 + _streams.s1_id1;
}

int o1S2C0_Comp_Compute(PS_STREAMS _streams)
{
    return (o1S2C0_Comp_var1 + _streams.s1_id0) + o1S2C0_o0S8C0_SubComp_Compute(_streams);
}

int o2S2C1_SubComp_Compute(PS_STREAMS _streams)
{
    return o2S2C1_SubComp_var1 + _streams.s1_id2;
}

void frag_main()
{
    PS_STREAMS _streams = { 0, 0, 0 };
    _streams.s1_id0 = PS_IN_s1;
    _streams.s1_id1 = PS_IN_s1_1;
    _streams.s1_id2 = PS_IN_s1_2;
    int i = o1S2C0_Comp_Compute(_streams) + o2S2C1_SubComp_Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_s1 = stage_input.PS_IN_s1;
    PS_IN_s1_1 = stage_input.PS_IN_s1_1;
    PS_IN_s1_2 = stage_input.PS_IN_s1_2;
    frag_main();
}
