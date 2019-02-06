struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 Position_id1;
    float2 TexCoord_id2;
};

static const VS_STREAMS _36 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx };

cbuffer PerDraw
{
    column_major float4x4 SpriteBase_MatrixTransform;
};

static float4 gl_Position;
static float4 VS_IN_POSITION;
static float2 VS_IN_TEXCOORD0;
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
    _streams.Position_id1 = VS_IN_POSITION;
    _streams.TexCoord_id2 = VS_IN_TEXCOORD0;
    _streams.ShadingPosition_id0 = mul(_streams.Position_id1, SpriteBase_MatrixTransform);
    gl_Position = _streams.ShadingPosition_id0;
    VS_OUT_TexCoord = _streams.TexCoord_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_POSITION = stage_input.VS_IN_POSITION;
    VS_IN_TEXCOORD0 = stage_input.VS_IN_TEXCOORD0;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
