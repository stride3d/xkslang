struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 PositionWS_id1;
    float shadingColorAlpha_id2;
};

static float4 VS_IN_ShadingPosition;
static float4 VS_IN_PositionWS;
static float VS_IN_shadingColorAlpha;
static float4 VS_OUT_ShadingPosition;
static float4 VS_OUT_PositionWS;
static float VS_OUT_shadingColorAlpha;

struct SPIRV_Cross_Input
{
    float4 VS_IN_ShadingPosition : SV_Position;
    float4 VS_IN_PositionWS : POSITION_WS;
    float VS_IN_shadingColorAlpha : TEXCOORD2;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
    float4 VS_OUT_PositionWS : POSITION_WS;
    float VS_OUT_shadingColorAlpha : TEXCOORD2;
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
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f };
    _streams.ShadingPosition_id0 = VS_IN_ShadingPosition;
    _streams.PositionWS_id1 = VS_IN_PositionWS;
    _streams.shadingColorAlpha_id2 = VS_IN_shadingColorAlpha;
    ShaderBase_VSMain();
    o1S2C1_IStreamInitializer_ResetStream();
    o0S2C0_IMaterialSurface_Compute();
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_PositionWS = _streams.PositionWS_id1;
    VS_OUT_shadingColorAlpha = _streams.shadingColorAlpha_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_ShadingPosition = stage_input.VS_IN_ShadingPosition;
    VS_IN_PositionWS = stage_input.VS_IN_PositionWS;
    VS_IN_shadingColorAlpha = stage_input.VS_IN_shadingColorAlpha;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_shadingColorAlpha = VS_OUT_shadingColorAlpha;
    return stage_output;
}
