struct VS_STREAMS
{
    float matAmbientOcclusion_id0;
    float matAmbientOcclusionDirectLightingFactor_id1;
    float matCavity_id2;
    float matCavityDiffuse_id3;
    float matCavitySpecular_id4;
    float2 matDiffuseSpecularAlphaBlend_id5;
    float3 matAlphaBlendColor_id6;
    float4 PositionWS_id7;
    float4 ShadingPosition_id8;
    float3x3 tangentToWorld_id9;
    float2 TexCoord_id10;
};

static float VS_IN_matAmbientOcclusion;
static float VS_IN_matAmbientOcclusionDirectLightingFactor;
static float VS_IN_matCavity;
static float VS_IN_matCavityDiffuse;
static float VS_IN_matCavitySpecular;
static float2 VS_IN_matDiffuseSpecularAlphaBlend;
static float3 VS_IN_matAlphaBlendColor;
static float4 VS_IN_PositionWS;
static float4 VS_IN_ShadingPosition;
static float3x3 VS_IN_tangentToWorld;
static float2 VS_IN_TexCoord;
static float VS_OUT_matAmbientOcclusion;
static float VS_OUT_matAmbientOcclusionDirectLightingFactor;
static float VS_OUT_matCavity;
static float VS_OUT_matCavityDiffuse;
static float VS_OUT_matCavitySpecular;
static float2 VS_OUT_matDiffuseSpecularAlphaBlend;
static float3 VS_OUT_matAlphaBlendColor;
static float4 VS_OUT_PositionWS;
static float4 VS_OUT_ShadingPosition;
static float3x3 VS_OUT_tangentToWorld;
static float2 VS_OUT_TexCoord;

struct SPIRV_Cross_Input
{
    float VS_IN_matAmbientOcclusion : TEXCOORD0;
    float VS_IN_matAmbientOcclusionDirectLightingFactor : TEXCOORD1;
    float VS_IN_matCavity : TEXCOORD2;
    float VS_IN_matCavityDiffuse : TEXCOORD3;
    float VS_IN_matCavitySpecular : TEXCOORD4;
    float2 VS_IN_matDiffuseSpecularAlphaBlend : TEXCOORD5;
    float3 VS_IN_matAlphaBlendColor : TEXCOORD6;
    float4 VS_IN_PositionWS : POSITION_WS;
    float4 VS_IN_ShadingPosition : SV_Position;
    float3 VS_IN_tangentToWorld_0 : TEXCOORD9;
    float3 VS_IN_tangentToWorld_1 : TEXCOORD10;
    float3 VS_IN_tangentToWorld_2 : TEXCOORD11;
    float2 VS_IN_TexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float VS_OUT_matAmbientOcclusion : TEXCOORD0;
    float VS_OUT_matAmbientOcclusionDirectLightingFactor : TEXCOORD1;
    float VS_OUT_matCavity : TEXCOORD2;
    float VS_OUT_matCavityDiffuse : TEXCOORD3;
    float VS_OUT_matCavitySpecular : TEXCOORD4;
    float2 VS_OUT_matDiffuseSpecularAlphaBlend : TEXCOORD5;
    float3 VS_OUT_matAlphaBlendColor : TEXCOORD6;
    float4 VS_OUT_PositionWS : POSITION_WS;
    float4 VS_OUT_ShadingPosition : SV_Position;
    float3 VS_OUT_tangentToWorld_0 : TEXCOORD9;
    float3 VS_OUT_tangentToWorld_1 : TEXCOORD10;
    float3 VS_OUT_tangentToWorld_2 : TEXCOORD11;
    float2 VS_OUT_TexCoord : TEXCOORD0;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), float2(0.0f, 0.0f) };
    _streams.matAmbientOcclusion_id0 = VS_IN_matAmbientOcclusion;
    _streams.matAmbientOcclusionDirectLightingFactor_id1 = VS_IN_matAmbientOcclusionDirectLightingFactor;
    _streams.matCavity_id2 = VS_IN_matCavity;
    _streams.matCavityDiffuse_id3 = VS_IN_matCavityDiffuse;
    _streams.matCavitySpecular_id4 = VS_IN_matCavitySpecular;
    _streams.matDiffuseSpecularAlphaBlend_id5 = VS_IN_matDiffuseSpecularAlphaBlend;
    _streams.matAlphaBlendColor_id6 = VS_IN_matAlphaBlendColor;
    _streams.PositionWS_id7 = VS_IN_PositionWS;
    _streams.ShadingPosition_id8 = VS_IN_ShadingPosition;
    _streams.tangentToWorld_id9 = VS_IN_tangentToWorld;
    _streams.TexCoord_id10 = VS_IN_TexCoord;
    VS_OUT_matAmbientOcclusion = _streams.matAmbientOcclusion_id0;
    VS_OUT_matAmbientOcclusionDirectLightingFactor = _streams.matAmbientOcclusionDirectLightingFactor_id1;
    VS_OUT_matCavity = _streams.matCavity_id2;
    VS_OUT_matCavityDiffuse = _streams.matCavityDiffuse_id3;
    VS_OUT_matCavitySpecular = _streams.matCavitySpecular_id4;
    VS_OUT_matDiffuseSpecularAlphaBlend = _streams.matDiffuseSpecularAlphaBlend_id5;
    VS_OUT_matAlphaBlendColor = _streams.matAlphaBlendColor_id6;
    VS_OUT_PositionWS = _streams.PositionWS_id7;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id8;
    VS_OUT_tangentToWorld = _streams.tangentToWorld_id9;
    VS_OUT_TexCoord = _streams.TexCoord_id10;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_matAmbientOcclusion = stage_input.VS_IN_matAmbientOcclusion;
    VS_IN_matAmbientOcclusionDirectLightingFactor = stage_input.VS_IN_matAmbientOcclusionDirectLightingFactor;
    VS_IN_matCavity = stage_input.VS_IN_matCavity;
    VS_IN_matCavityDiffuse = stage_input.VS_IN_matCavityDiffuse;
    VS_IN_matCavitySpecular = stage_input.VS_IN_matCavitySpecular;
    VS_IN_matDiffuseSpecularAlphaBlend = stage_input.VS_IN_matDiffuseSpecularAlphaBlend;
    VS_IN_matAlphaBlendColor = stage_input.VS_IN_matAlphaBlendColor;
    VS_IN_PositionWS = stage_input.VS_IN_PositionWS;
    VS_IN_ShadingPosition = stage_input.VS_IN_ShadingPosition;
    VS_IN_tangentToWorld[0] = stage_input.VS_IN_tangentToWorld_0;
    VS_IN_tangentToWorld[1] = stage_input.VS_IN_tangentToWorld_1;
    VS_IN_tangentToWorld[2] = stage_input.VS_IN_tangentToWorld_2;
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_matAmbientOcclusion = VS_OUT_matAmbientOcclusion;
    stage_output.VS_OUT_matAmbientOcclusionDirectLightingFactor = VS_OUT_matAmbientOcclusionDirectLightingFactor;
    stage_output.VS_OUT_matCavity = VS_OUT_matCavity;
    stage_output.VS_OUT_matCavityDiffuse = VS_OUT_matCavityDiffuse;
    stage_output.VS_OUT_matCavitySpecular = VS_OUT_matCavitySpecular;
    stage_output.VS_OUT_matDiffuseSpecularAlphaBlend = VS_OUT_matDiffuseSpecularAlphaBlend;
    stage_output.VS_OUT_matAlphaBlendColor = VS_OUT_matAlphaBlendColor;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_tangentToWorld = VS_OUT_tangentToWorld;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
