struct VS_STREAMS
{
    float2 TexCoord_id0;
    float4 ShadingPosition_id1;
    float4 Position_id2;
};

static const VS_STREAMS _36 = { 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer PerDraw
{
    column_major float4x4 SpriteBase_MatrixTransform;
};

static float4 gl_Position;
static float2 VS_IN_TEXCOORD0;
static float4 VS_IN_POSITION;
static float2 VS_OUT_TexCoord;

struct SPIRV_Cross_Input
{
    float4 VS_IN_POSITION : POSITION;
    float2 VS_IN_TEXCOORD0 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    VS_STREAMS _streams = _36;
    _streams.TexCoord_id0 = VS_IN_TEXCOORD0;
    _streams.Position_id2 = VS_IN_POSITION;
    _streams.ShadingPosition_id1 = mul(_streams.Position_id2, SpriteBase_MatrixTransform);
    VS_OUT_TexCoord = _streams.TexCoord_id0;
    gl_Position = _streams.ShadingPosition_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TEXCOORD0 = stage_input.VS_IN_TEXCOORD0;
    VS_IN_POSITION = stage_input.VS_IN_POSITION;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
