struct VS_STREAMS
{
    float3 meshNormal_id0;
    float4 meshTangent_id1;
    float3 normalWS_id2;
    float4 ShadingPosition_id3;
};

static const VS_STREAMS _38 = { 0.0f.xxx, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxxx };

static float4 gl_Position;
static float3 VS_IN_NORMAL;
static float4 VS_IN_TANGENT;
static float4 VS_IN_SV_Position;
static float3 VS_OUT_meshNormal;
static float4 VS_OUT_meshTangent;

struct SPIRV_Cross_Input
{
    float3 VS_IN_NORMAL : NORMAL;
    float4 VS_IN_SV_Position : SV_Position;
    float4 VS_IN_TANGENT : TANGENT;
};

struct SPIRV_Cross_Output
{
    float3 VS_OUT_meshNormal : NORMAL;
    float4 VS_OUT_meshTangent : TANGENT;
    float4 gl_Position : SV_Position;
};

void ShaderBase_VSMain()
{
}

void NormalUpdate_GenerateNormal_VS(inout VS_STREAMS _streams)
{
    _streams.normalWS_id2 = 0.0f.xxx;
}

void vert_main()
{
    VS_STREAMS _streams = _38;
    _streams.meshNormal_id0 = VS_IN_NORMAL;
    _streams.meshTangent_id1 = VS_IN_TANGENT;
    _streams.ShadingPosition_id3 = VS_IN_SV_Position;
    ShaderBase_VSMain();
    NormalUpdate_GenerateNormal_VS(_streams);
    VS_OUT_meshNormal = _streams.meshNormal_id0;
    VS_OUT_meshTangent = _streams.meshTangent_id1;
    gl_Position = _streams.ShadingPosition_id3;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_NORMAL = stage_input.VS_IN_NORMAL;
    VS_IN_TANGENT = stage_input.VS_IN_TANGENT;
    VS_IN_SV_Position = stage_input.VS_IN_SV_Position;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_meshNormal = VS_OUT_meshNormal;
    stage_output.VS_OUT_meshTangent = VS_OUT_meshTangent;
    return stage_output;
}
