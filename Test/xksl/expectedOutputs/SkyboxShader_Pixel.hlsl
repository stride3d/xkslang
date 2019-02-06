struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float3 skyboxViewDirection_id2;
};

static const PS_STREAMS _69 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxx };

cbuffer Globals
{
    float SkyboxShader_Intensity;
    column_major float4x4 SkyboxShader_ProjectionInverse;
    column_major float4x4 SkyboxShader_ViewInverse;
    column_major float4x4 SkyboxShader_SkyMatrix;
};
TextureCube<float4> SkyboxShader_CubeMap;
SamplerState Texturing_LinearSampler;

static float4 PS_IN_SV_Position;
static float3 PS_IN_SKYBOXVIEWDIRECTION;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float3 PS_IN_SKYBOXVIEWDIRECTION : SKYBOXVIEWDIRECTION;
    float4 PS_IN_SV_Position : SV_Position;
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
    PS_STREAMS _streams = _69;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.skyboxViewDirection_id2 = PS_IN_SKYBOXVIEWDIRECTION;
    _streams.ColorTarget_id1 = SkyboxShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    PS_IN_SKYBOXVIEWDIRECTION = stage_input.PS_IN_SKYBOXVIEWDIRECTION;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
