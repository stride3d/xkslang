struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 Position_id1;
    float2 TexCoord_id2;
};

cbuffer PerDraw
{
    column_major float4x4 SpriteBase_MatrixTransform;
};

static float4 VS_IN_Position;
static float2 VS_IN_TexCoord;
static float4 VS_OUT_ShadingPosition;
static float2 VS_OUT_TexCoord;

struct SPIRV_Cross_Input
{
    float4 VS_IN_Position : POSITION;
    float2 VS_IN_TexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
    float2 VS_OUT_TexCoord : TEXCOORD0;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx };
    _streams.Position_id1 = VS_IN_Position;
    _streams.TexCoord_id2 = VS_IN_TexCoord;
    _streams.ShadingPosition_id0 = mul(_streams.Position_id1, SpriteBase_MatrixTransform);
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_TexCoord = _streams.TexCoord_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_Position = stage_input.VS_IN_Position;
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
