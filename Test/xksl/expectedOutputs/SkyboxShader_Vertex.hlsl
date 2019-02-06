struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float3 skyboxViewDirection_id1;
    float4 Position_id2;
};

static const VS_STREAMS _92 = { 0.0f.xxxx, 0.0f.xxx, 0.0f.xxxx };

cbuffer Globals
{
    float SkyboxShader_Intensity;
    column_major float4x4 SkyboxShader_ProjectionInverse;
    column_major float4x4 SkyboxShader_ViewInverse;
    column_major float4x4 SkyboxShader_SkyMatrix;
};
cbuffer PerDraw
{
    column_major float4x4 SpriteBase_MatrixTransform;
};

static float4 gl_Position;
static float4 VS_IN_POSITION;
static float3 VS_OUT_skyboxViewDirection;

struct SPIRV_Cross_Input
{
    float4 VS_IN_POSITION : POSITION;
};

struct SPIRV_Cross_Output
{
    float3 VS_OUT_skyboxViewDirection : SKYBOXVIEWDIRECTION;
    float4 gl_Position : SV_Position;
};

void SpriteBase_VSMain(inout VS_STREAMS _streams)
{
    _streams.ShadingPosition_id0 = mul(_streams.Position_id2, SpriteBase_MatrixTransform);
}

void vert_main()
{
    VS_STREAMS _streams = _92;
    _streams.Position_id2 = VS_IN_POSITION;
    SpriteBase_VSMain(_streams);
    float4 screenPosition = _streams.ShadingPosition_id0 / _streams.ShadingPosition_id0.w.xxxx;
    float4 position = float4(screenPosition.x, screenPosition.y, 1.0f, 1.0f);
    float3 directionVS = mul(position, SkyboxShader_ProjectionInverse).xyz;
    float3 directionWS = mul(float4(directionVS, 0.0f), SkyboxShader_ViewInverse).xyz;
    _streams.skyboxViewDirection_id1 = mul(directionWS, float3x3(float3(SkyboxShader_SkyMatrix[0].x, SkyboxShader_SkyMatrix[0].y, SkyboxShader_SkyMatrix[0].z), float3(SkyboxShader_SkyMatrix[1].x, SkyboxShader_SkyMatrix[1].y, SkyboxShader_SkyMatrix[1].z), float3(SkyboxShader_SkyMatrix[2].x, SkyboxShader_SkyMatrix[2].y, SkyboxShader_SkyMatrix[2].z)));
    gl_Position = _streams.ShadingPosition_id0;
    VS_OUT_skyboxViewDirection = _streams.skyboxViewDirection_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_POSITION = stage_input.VS_IN_POSITION;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_skyboxViewDirection = VS_OUT_skyboxViewDirection;
    return stage_output;
}
