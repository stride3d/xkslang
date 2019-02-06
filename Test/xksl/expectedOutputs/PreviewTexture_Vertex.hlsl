struct VS_STREAMS
{
    float4 Color_id0;
    float4 ColorAdd_id1;
    float Swizzle_id2;
    float4 ShadingPosition_id3;
    float4 Position_id4;
    float2 TexCoord_id5;
};

static const VS_STREAMS _90 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx };

cbuffer PerDraw
{
    column_major float4x4 SpriteBase_MatrixTransform;
};

static float4 gl_Position;
static float4 VS_IN_COLOR;
static float4 VS_IN_COLOR1;
static float VS_IN_BATCH_SWIZZLE;
static float4 VS_IN_POSITION;
static float2 VS_IN_TEXCOORD0;
static float4 VS_OUT_Color;
static float4 VS_OUT_ColorAdd;
static float VS_OUT_Swizzle;
static float2 VS_OUT_TexCoord;

struct SPIRV_Cross_Input
{
    float VS_IN_BATCH_SWIZZLE : BATCH_SWIZZLE;
    float4 VS_IN_COLOR : COLOR;
    float4 VS_IN_COLOR1 : COLOR1;
    float4 VS_IN_POSITION : POSITION;
    float2 VS_IN_TEXCOORD0 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_Color : COLOR;
    float4 VS_OUT_ColorAdd : COLOR1;
    float VS_OUT_Swizzle : BATCH_SWIZZLE;
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void SpriteBase_VSMain(inout VS_STREAMS _streams)
{
    _streams.ShadingPosition_id3 = mul(_streams.Position_id4, SpriteBase_MatrixTransform);
}

float4 ColorUtility_ToLinear(float4 sRGBa)
{
    float3 sRGB = sRGBa.xyz;
    return float4(sRGB * ((sRGB * ((sRGB * 0.305306017398834228515625f) + 0.6821711063385009765625f.xxx)) + 0.01252287812530994415283203125f.xxx), sRGBa.w);
}

void vert_main()
{
    VS_STREAMS _streams = _90;
    _streams.Color_id0 = VS_IN_COLOR;
    _streams.ColorAdd_id1 = VS_IN_COLOR1;
    _streams.Swizzle_id2 = VS_IN_BATCH_SWIZZLE;
    _streams.Position_id4 = VS_IN_POSITION;
    _streams.TexCoord_id5 = VS_IN_TEXCOORD0;
    SpriteBase_VSMain(_streams);
    if (false)
    {
        float4 param = _streams.Color_id0;
        _streams.Color_id0 = ColorUtility_ToLinear(param);
    }
    VS_OUT_Color = _streams.Color_id0;
    VS_OUT_ColorAdd = _streams.ColorAdd_id1;
    VS_OUT_Swizzle = _streams.Swizzle_id2;
    gl_Position = _streams.ShadingPosition_id3;
    VS_OUT_TexCoord = _streams.TexCoord_id5;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_COLOR = stage_input.VS_IN_COLOR;
    VS_IN_COLOR1 = stage_input.VS_IN_COLOR1;
    VS_IN_BATCH_SWIZZLE = stage_input.VS_IN_BATCH_SWIZZLE;
    VS_IN_POSITION = stage_input.VS_IN_POSITION;
    VS_IN_TEXCOORD0 = stage_input.VS_IN_TEXCOORD0;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_Color = VS_OUT_Color;
    stage_output.VS_OUT_ColorAdd = VS_OUT_ColorAdd;
    stage_output.VS_OUT_Swizzle = VS_OUT_Swizzle;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
