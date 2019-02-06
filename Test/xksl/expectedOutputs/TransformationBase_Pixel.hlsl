struct PS_STREAMS
{
    float4 ShadingPosition_id0;
};

static const PS_STREAMS _17 = { 0.0f.xxxx };

static float4 PS_IN_SV_Position;

struct SPIRV_Cross_Input
{
    float4 PS_IN_SV_Position : SV_Position;
};

void frag_main()
{
    PS_STREAMS _streams = _17;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    frag_main();
}
