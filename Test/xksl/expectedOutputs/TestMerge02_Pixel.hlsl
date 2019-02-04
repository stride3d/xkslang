struct PS_STREAMS
{
    int sa1_id0;
};

static const PS_STREAMS _24 = { 0 };

cbuffer Globals
{
    int shaderA_Var0;
};

static int PS_IN_SA1;

struct SPIRV_Cross_Input
{
    int PS_IN_SA1 : SA1;
};

void frag_main()
{
    PS_STREAMS _streams = _24;
    _streams.sa1_id0 = PS_IN_SA1;
    int a = ((shaderA_Var0 + _streams.sa1_id0) + 2) + 3;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SA1 = stage_input.PS_IN_SA1;
    frag_main();
}
