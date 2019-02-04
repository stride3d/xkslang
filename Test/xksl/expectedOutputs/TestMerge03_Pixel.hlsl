struct PS_STREAMS
{
    float sbase1_id0;
};

static const PS_STREAMS _32 = { 0.0f };

cbuffer Globals
{
    int Base_Var1;
    float Base_Var2;
};

static float PS_IN_SBASE1;

struct SPIRV_Cross_Input
{
    float PS_IN_SBASE1 : SBASE1;
};

void frag_main()
{
    PS_STREAMS _streams = _32;
    _streams.sbase1_id0 = PS_IN_SBASE1;
    int i = int(((_streams.sbase1_id0 + float(Base_Var1)) + Base_Var2) + 2.0f);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SBASE1 = stage_input.PS_IN_SBASE1;
    frag_main();
}
