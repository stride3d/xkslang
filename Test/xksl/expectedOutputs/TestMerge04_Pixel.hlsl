struct PS_STREAMS
{
    float sbase1_id0;
    int sa1_id1;
};

static const PS_STREAMS _40 = { 0.0f, 0 };

cbuffer Globals
{
    int Base_Var1;
};
cbuffer C1
{
    int shaderA_Var1;
};

static float PS_IN_SBASE1;
static int PS_IN_SA1;

struct SPIRV_Cross_Input
{
    int PS_IN_SA1 : SA1;
    float PS_IN_SBASE1 : SBASE1;
};

void frag_main()
{
    PS_STREAMS _streams = _40;
    _streams.sbase1_id0 = PS_IN_SBASE1;
    _streams.sa1_id1 = PS_IN_SA1;
    int i = int(((((2.0f + _streams.sbase1_id0) + float(shaderA_Var1)) + float(Base_Var1)) + 5.0f) + float(_streams.sa1_id1));
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SBASE1 = stage_input.PS_IN_SBASE1;
    PS_IN_SA1 = stage_input.PS_IN_SA1;
    frag_main();
}
