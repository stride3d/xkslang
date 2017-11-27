struct PS_STREAMS
{
    float2x3 aMat23_id0;
};

static float2x3 PS_IN_aMat23;

struct SPIRV_Cross_Input
{
    float2x3 PS_IN_aMat23 : AMAT23;
};

void frag_main()
{
    PS_STREAMS _streams = { float2x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)) };
    _streams.aMat23_id0 = PS_IN_aMat23;
    float f = _streams.aMat23_id0[0].x;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_aMat23 = stage_input.PS_IN_aMat23;
    frag_main();
}
