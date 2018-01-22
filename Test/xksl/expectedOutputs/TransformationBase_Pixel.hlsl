struct PS_STREAMS
{
    float4 ShadingPosition_id0;
};

static float4 PS_IN_ShadingPosition;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
};

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    frag_main();
}
