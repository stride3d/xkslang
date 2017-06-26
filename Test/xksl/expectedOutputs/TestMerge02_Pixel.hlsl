struct PS_STREAMS
{
    int sa1_id0;
};

cbuffer Globals
{
    int shaderA_Var0;
};

static int PS_IN_sa1;

struct SPIRV_Cross_Input
{
    int PS_IN_sa1 : TEXCOORD0;
};

void frag_main()
{
    PS_STREAMS _streams = { 0 };
    _streams.sa1_id0 = PS_IN_sa1;
    int a = ((shaderA_Var0 + _streams.sa1_id0) + 2) + 3;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_sa1 = stage_input.PS_IN_sa1;
    frag_main();
}
