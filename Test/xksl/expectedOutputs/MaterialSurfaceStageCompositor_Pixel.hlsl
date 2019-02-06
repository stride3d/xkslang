struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float3 viewWS_id2;
    float4 PositionWS_id3;
    float3 shadingColor_id4;
    float shadingColorAlpha_id5;
};

static const PS_STREAMS _66 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxx, 0.0f };

cbuffer PerView
{
    column_major float4x4 Transformation_View;
    column_major float4x4 Transformation_ViewInverse;
    column_major float4x4 Transformation_Projection;
    column_major float4x4 Transformation_ProjectionInverse;
    column_major float4x4 Transformation_ViewProjection;
    float2 Transformation_ProjScreenRay;
    float4 Transformation_Eye;
};

static float4 PS_IN_SV_Position;
static float4 PS_IN_POSITION_WS;
static float PS_IN_SHADINGCOLORALPHA;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_POSITION_WS : POSITION_WS;
    float PS_IN_SHADINGCOLORALPHA : SHADINGCOLORALPHA;
    float4 PS_IN_SV_Position : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void ShaderBase_PSMain()
{
}

void o3S41C1_IStreamInitializer_ResetStream()
{
}

void o2S41C0_IMaterialSurface_Compute()
{
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id2 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id3.xyz);
    _streams.shadingColor_id4 = 0.0f.xxx;
    o3S41C1_IStreamInitializer_ResetStream();
    o2S41C0_IMaterialSurface_Compute();
    return float4(_streams.shadingColor_id4, _streams.shadingColorAlpha_id5);
}

void frag_main()
{
    PS_STREAMS _streams = _66;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.PositionWS_id3 = PS_IN_POSITION_WS;
    _streams.shadingColorAlpha_id5 = PS_IN_SHADINGCOLORALPHA;
    ShaderBase_PSMain();
    float4 _36 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id1 = _36;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    PS_IN_POSITION_WS = stage_input.PS_IN_POSITION_WS;
    PS_IN_SHADINGCOLORALPHA = stage_input.PS_IN_SHADINGCOLORALPHA;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
