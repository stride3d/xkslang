struct VS_STREAMS
{
    float4 Position_id0;
    float4 ScreenPosition_id1;
    float ZDepth_id2;
    float4 ShadingPosition_id3;
    float2 TexCoord_id4;
    float4 LocalColor_id5;
};

cbuffer PerMaterial
{
    float4 ParticleBase_ColorScale;
    float ParticleBase_AlphaAdditive;
    float ParticleBase_ZOffset;
    float ParticleBase_SoftEdgeInverseDistance;
};
cbuffer PerView
{
    column_major float4x4 ParticleUtilities_ViewMatrix;
    column_major float4x4 ParticleUtilities_ProjectionMatrix;
    column_major float4x4 ParticleUtilities_ViewProjectionMatrix;
    float4 ParticleUtilities_ViewFrustum;
    float4 ParticleUtilities_Viewport;
};

static float4 VS_IN_Position;
static float2 VS_IN_TexCoord;
static float4 VS_IN_LocalColor;
static float4 VS_OUT_ScreenPosition;
static float VS_OUT_ZDepth;
static float4 VS_OUT_ShadingPosition;
static float2 VS_OUT_TexCoord;
static float4 VS_OUT_LocalColor;

struct SPIRV_Cross_Input
{
    float4 VS_IN_Position : POSITION;
    float2 VS_IN_TexCoord : TEXCOORD0;
    float4 VS_IN_LocalColor : COLOR0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ScreenPosition : SCREEN_POSITION;
    float VS_OUT_ZDepth : Z_DEPTH_VALUE;
    float4 VS_OUT_ShadingPosition : SV_Position;
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float4 VS_OUT_LocalColor : COLOR0;
};

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.Position_id0 = VS_IN_Position;
    _streams.TexCoord_id4 = VS_IN_TexCoord;
    _streams.LocalColor_id5 = VS_IN_LocalColor;
    float4 worldPos = _streams.Position_id0;
    float4 viewPos = mul(worldPos, ParticleUtilities_ViewMatrix);
    _streams.ShadingPosition_id3 = mul(viewPos, ParticleUtilities_ProjectionMatrix);
    _streams.ScreenPosition_id1 = _streams.ShadingPosition_id3;
    viewPos.w = 1.0f;
    viewPos.z += ParticleBase_ZOffset;
    _streams.ZDepth_id2 = viewPos.z;
    float4 viewProjPos = mul(viewPos, ParticleUtilities_ProjectionMatrix);
    _streams.ShadingPosition_id3.z = (viewProjPos.z / viewProjPos.w) * _streams.ShadingPosition_id3.w;
    VS_OUT_ScreenPosition = _streams.ScreenPosition_id1;
    VS_OUT_ZDepth = _streams.ZDepth_id2;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
    VS_OUT_TexCoord = _streams.TexCoord_id4;
    VS_OUT_LocalColor = _streams.LocalColor_id5;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_Position = stage_input.VS_IN_Position;
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    VS_IN_LocalColor = stage_input.VS_IN_LocalColor;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ScreenPosition = VS_OUT_ScreenPosition;
    stage_output.VS_OUT_ZDepth = VS_OUT_ZDepth;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    stage_output.VS_OUT_LocalColor = VS_OUT_LocalColor;
    return stage_output;
}
