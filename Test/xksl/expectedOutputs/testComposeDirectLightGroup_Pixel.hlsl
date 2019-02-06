struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ScreenPosition_id1;
};

static const PS_STREAMS _31 = { 0.0f.xxxx, 0.0f.xxxx };

static float4 PS_IN_SV_Position;
static float4 PS_IN_SCREENPOSITION;

struct SPIRV_Cross_Input
{
    float4 PS_IN_SCREENPOSITION : SCREENPOSITION;
    float4 PS_IN_SV_Position : SV_Position;
};

void o1S2C0_ShaderBase_PSMain()
{
}

void frag_main()
{
    PS_STREAMS _streams = _31;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.ScreenPosition_id1 = PS_IN_SCREENPOSITION;
    _streams.ScreenPosition_id1 /= _streams.ScreenPosition_id1.w.xxxx;
    o1S2C0_ShaderBase_PSMain();
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    PS_IN_SCREENPOSITION = stage_input.PS_IN_SCREENPOSITION;
    frag_main();
}
