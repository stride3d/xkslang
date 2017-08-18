struct VS_STREAMS
{
    float3 meshNormal_id0;
    float4 meshTangent_id1;
    float3 normalWS_id2;
    float4 ShadingPosition_id3;
};

static float3 VS_IN_meshNormal;
static float4 VS_IN_meshTangent;
static float4 VS_IN_ShadingPosition;
static float3 VS_OUT_meshNormal;
static float4 VS_OUT_meshTangent;
static float4 VS_OUT_ShadingPosition;

struct SPIRV_Cross_Input
{
    float3 VS_IN_meshNormal : NORMAL;
    float4 VS_IN_meshTangent : TANGENT;
    float4 VS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float3 VS_OUT_meshNormal : NORMAL;
    float4 VS_OUT_meshTangent : TANGENT;
    float4 VS_OUT_ShadingPosition : SV_Position;
};

void ShaderBase_VSMain()
{
}

void NormalUpdate_GenerateNormal_VS(inout VS_STREAMS _streams)
{
    _streams.normalWS_id2 = float3(0.0f, 0.0f, 0.0f);
}

void vert_main()
{
    VS_STREAMS _streams = { float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.meshNormal_id0 = VS_IN_meshNormal;
    _streams.meshTangent_id1 = VS_IN_meshTangent;
    _streams.ShadingPosition_id3 = VS_IN_ShadingPosition;
    ShaderBase_VSMain();
    NormalUpdate_GenerateNormal_VS(_streams);
    VS_OUT_meshNormal = _streams.meshNormal_id0;
    VS_OUT_meshTangent = _streams.meshTangent_id1;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_meshNormal = stage_input.VS_IN_meshNormal;
    VS_IN_meshTangent = stage_input.VS_IN_meshTangent;
    VS_IN_ShadingPosition = stage_input.VS_IN_ShadingPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_meshNormal = VS_OUT_meshNormal;
    stage_output.VS_OUT_meshTangent = VS_OUT_meshTangent;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    return stage_output;
}
