struct PS_STREAMS
{
    float sbase1_id0;
};

cbuffer Globals
{
    int Base_Var1;
    float Base_Var2;
};

static float PS_IN_sbase1;

struct SPIRV_Cross_Input
{
    float PS_IN_sbase1 : SBASE1;
};

void frag_main()
{
    PS_STREAMS _streams = { 0.0f };
    _streams.sbase1_id0 = PS_IN_sbase1;
    int i = int(((_streams.sbase1_id0 + float(Base_Var1)) + Base_Var2) + 2.0f);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_sbase1 = stage_input.PS_IN_sbase1;
    frag_main();
}
