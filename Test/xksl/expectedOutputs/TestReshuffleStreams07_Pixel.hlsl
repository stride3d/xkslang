struct PS_STREAMS
{
    float2x3 aMat23_id0;
};

static const PS_STREAMS _25 = { float2x3(0.0f.xxx, 0.0f.xxx) };

static float2x3 PS_IN_AMAT23;

struct SPIRV_Cross_Input
{
    float2x3 PS_IN_AMAT23 : AMAT23;
};

void frag_main()
{
    PS_STREAMS _streams = _25;
    _streams.aMat23_id0 = PS_IN_AMAT23;
    float f = _streams.aMat23_id0[0].x;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_AMAT23 = stage_input.PS_IN_AMAT23;
    frag_main();
}
