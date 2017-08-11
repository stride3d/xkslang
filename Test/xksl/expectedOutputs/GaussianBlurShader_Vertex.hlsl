struct VS_STREAMS
{
    float2 TexCoord_id0;
    float4 ShadingPosition_id1;
    float4 Position_id2;
};

cbuffer PerDraw
{
    float4x4 SpriteBase_MatrixTransform;
};

static float2 VS_IN_TexCoord;
static float4 VS_IN_Position;
static float2 VS_OUT_TexCoord;
static float4 VS_OUT_ShadingPosition;

struct SPIRV_Cross_Input
{
    float2 VS_IN_TexCoord : TEXCOORD0;
    float4 VS_IN_Position : POSITION;
};

struct SPIRV_Cross_Output
{
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float4 VS_OUT_ShadingPosition : SV_Position;
};

void vert_main()
{
    VS_STREAMS _streams = { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.TexCoord_id0 = VS_IN_TexCoord;
    _streams.Position_id2 = VS_IN_Position;
    _streams.ShadingPosition_id1 = mul(_streams.Position_id2, SpriteBase_MatrixTransform);
    VS_OUT_TexCoord = _streams.TexCoord_id0;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    VS_IN_Position = stage_input.VS_IN_Position;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    return stage_output;
}
