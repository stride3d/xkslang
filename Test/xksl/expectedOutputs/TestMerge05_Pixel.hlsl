struct PS_STREAMS
{
    int sbase1_id0;
};

cbuffer Globals
{
    int Base_Var1;
};

static int PS_IN_sbase1;

struct SPIRV_Cross_Input
{
    int PS_IN_sbase1 : SBASE1;
};

int shaderA_Compute1(PS_STREAMS _streams)
{
    return Base_Var1 + _streams.sbase1_id0;
}

int shaderA_Compute2(PS_STREAMS _streams)
{
    return shaderA_Compute1(_streams);
}

void frag_main()
{
    PS_STREAMS _streams = { 0 };
    _streams.sbase1_id0 = PS_IN_sbase1;
    int i = shaderA_Compute1(_streams) + shaderA_Compute2(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_sbase1 = stage_input.PS_IN_sbase1;
    frag_main();
}
