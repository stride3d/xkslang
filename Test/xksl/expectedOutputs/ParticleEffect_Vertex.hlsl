struct VS_STREAMS
{
    float4 Position_id0;
    float4 ScreenPosition_id1;
    float ZDepth_id2;
    float4 ShadingPosition_id3;
    float2 TexCoord_id4;
    float4 LocalColor_id5;
};

static const VS_STREAMS _84 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx };

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

static float4 gl_Position;
static float4 VS_IN_POSITION;
static float2 VS_IN_TEXCOORD0;
static float4 VS_IN_COLOR0;
static float4 VS_OUT_ScreenPosition;
static float VS_OUT_ZDepth;
static float2 VS_OUT_TexCoord;
static float4 VS_OUT_LocalColor;

struct SPIRV_Cross_Input
{
    float4 VS_IN_COLOR0 : COLOR0;
    float4 VS_IN_POSITION : POSITION;
    float2 VS_IN_TEXCOORD0 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_LocalColor : COLOR0;
    float4 VS_OUT_ScreenPosition : SCREEN_POSITION;
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float VS_OUT_ZDepth : Z_DEPTH_VALUE;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    VS_STREAMS _streams = _84;
    _streams.Position_id0 = VS_IN_POSITION;
    _streams.TexCoord_id4 = VS_IN_TEXCOORD0;
    _streams.LocalColor_id5 = VS_IN_COLOR0;
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
    gl_Position = _streams.ShadingPosition_id3;
    VS_OUT_TexCoord = _streams.TexCoord_id4;
    VS_OUT_LocalColor = _streams.LocalColor_id5;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_POSITION = stage_input.VS_IN_POSITION;
    VS_IN_TEXCOORD0 = stage_input.VS_IN_TEXCOORD0;
    VS_IN_COLOR0 = stage_input.VS_IN_COLOR0;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_ScreenPosition = VS_OUT_ScreenPosition;
    stage_output.VS_OUT_ZDepth = VS_OUT_ZDepth;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    stage_output.VS_OUT_LocalColor = VS_OUT_LocalColor;
    return stage_output;
}
