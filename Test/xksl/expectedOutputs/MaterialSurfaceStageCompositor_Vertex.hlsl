struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 PositionWS_id1;
    float shadingColorAlpha_id2;
};

static const VS_STREAMS _37 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f };

static float4 gl_Position;
static float4 VS_IN_SV_Position;
static float4 VS_IN_POSITION_WS;
static float VS_IN_SHADINGCOLORALPHA;
static float4 VS_OUT_PositionWS;
static float VS_OUT_shadingColorAlpha;

struct SPIRV_Cross_Input
{
    float4 VS_IN_POSITION_WS : POSITION_WS;
    float VS_IN_SHADINGCOLORALPHA : SHADINGCOLORALPHA;
    float4 VS_IN_SV_Position : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_PositionWS : POSITION_WS;
    float VS_OUT_shadingColorAlpha : SHADINGCOLORALPHA;
    float4 gl_Position : SV_Position;
};

void ShaderBase_VSMain()
{
}

void o1S2C1_IStreamInitializer_ResetStream()
{
}

void o0S2C0_IMaterialSurface_Compute()
{
}

void vert_main()
{
    VS_STREAMS _streams = _37;
    _streams.ShadingPosition_id0 = VS_IN_SV_Position;
    _streams.PositionWS_id1 = VS_IN_POSITION_WS;
    _streams.shadingColorAlpha_id2 = VS_IN_SHADINGCOLORALPHA;
    ShaderBase_VSMain();
    o1S2C1_IStreamInitializer_ResetStream();
    o0S2C0_IMaterialSurface_Compute();
    gl_Position = _streams.ShadingPosition_id0;
    VS_OUT_PositionWS = _streams.PositionWS_id1;
    VS_OUT_shadingColorAlpha = _streams.shadingColorAlpha_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_SV_Position = stage_input.VS_IN_SV_Position;
    VS_IN_POSITION_WS = stage_input.VS_IN_POSITION_WS;
    VS_IN_SHADINGCOLORALPHA = stage_input.VS_IN_SHADINGCOLORALPHA;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_shadingColorAlpha = VS_OUT_shadingColorAlpha;
    return stage_output;
}
