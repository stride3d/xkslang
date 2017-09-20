struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ScreenPosition_id1;
};

static float4 PS_IN_ShadingPosition;
static float4 PS_IN_ScreenPosition;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float4 PS_IN_ScreenPosition : SCREENPOSITION;
};

void o1S2C0_ShaderBase_PSMain()
{
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.ScreenPosition_id1 = PS_IN_ScreenPosition;
    _streams.ScreenPosition_id1 /= float4(_streams.ScreenPosition_id1.w, _streams.ScreenPosition_id1.w, _streams.ScreenPosition_id1.w, _streams.ScreenPosition_id1.w);
    o1S2C0_ShaderBase_PSMain();
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_ScreenPosition = stage_input.PS_IN_ScreenPosition;
    frag_main();
}
