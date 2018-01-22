struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float3 skyboxViewDirection_id2;
};

cbuffer Globals
{
    float SkyboxShader_Intensity;
    column_major float4x4 SkyboxShader_ProjectionInverse;
    column_major float4x4 SkyboxShader_ViewInverse;
    column_major float4x4 SkyboxShader_SkyMatrix;
};
TextureCube<float4> SkyboxShader_CubeMap;
SamplerState Texturing_LinearSampler;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_skyboxViewDirection;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_skyboxViewDirection : SKYBOXVIEWDIRECTION;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float4 SkyboxShader_Shading(PS_STREAMS _streams)
{
    float3 samplingDir = normalize(_streams.skyboxViewDirection_id2);
    float3 color = SkyboxShader_CubeMap.Sample(Texturing_LinearSampler, float3(samplingDir.x, samplingDir.y, -samplingDir.z)).xyz;
    return float4(color * SkyboxShader_Intensity, 1.0f);
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxx };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.skyboxViewDirection_id2 = PS_IN_skyboxViewDirection;
    _streams.ColorTarget_id1 = SkyboxShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_skyboxViewDirection = stage_input.PS_IN_skyboxViewDirection;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
