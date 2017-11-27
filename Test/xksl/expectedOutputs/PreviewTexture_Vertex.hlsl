struct VS_STREAMS
{
    float4 Color_id0;
    float4 ColorAdd_id1;
    float Swizzle_id2;
    float4 ShadingPosition_id3;
    float4 Position_id4;
    float2 TexCoord_id5;
};

cbuffer PerDraw
{
    column_major float4x4 SpriteBase_MatrixTransform;
};

static float4 VS_IN_Color;
static float4 VS_IN_ColorAdd;
static float VS_IN_Swizzle;
static float4 VS_IN_Position;
static float2 VS_IN_TexCoord;
static float4 VS_OUT_Color;
static float4 VS_OUT_ColorAdd;
static float VS_OUT_Swizzle;
static float4 VS_OUT_ShadingPosition;
static float2 VS_OUT_TexCoord;

struct SPIRV_Cross_Input
{
    float4 VS_IN_Color : COLOR;
    float4 VS_IN_ColorAdd : COLOR1;
    float VS_IN_Swizzle : BATCH_SWIZZLE;
    float4 VS_IN_Position : POSITION;
    float2 VS_IN_TexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_Color : COLOR;
    float4 VS_OUT_ColorAdd : COLOR1;
    float VS_OUT_Swizzle : BATCH_SWIZZLE;
    float4 VS_OUT_ShadingPosition : SV_Position;
    float2 VS_OUT_TexCoord : TEXCOORD0;
};

void SpriteBase_VSMain(inout VS_STREAMS _streams)
{
    _streams.ShadingPosition_id3 = mul(_streams.Position_id4, SpriteBase_MatrixTransform);
}

float4 ColorUtility_ToLinear(float4 sRGBa)
{
    float3 sRGB = sRGBa.xyz;
    return float4(sRGB * ((sRGB * ((sRGB * 0.305306017398834228515625f) + float3(0.6821711063385009765625f, 0.6821711063385009765625f, 0.6821711063385009765625f))) + float3(0.01252287812530994415283203125f, 0.01252287812530994415283203125f, 0.01252287812530994415283203125f)), sRGBa.w);
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) };
    _streams.Color_id0 = VS_IN_Color;
    _streams.ColorAdd_id1 = VS_IN_ColorAdd;
    _streams.Swizzle_id2 = VS_IN_Swizzle;
    _streams.Position_id4 = VS_IN_Position;
    _streams.TexCoord_id5 = VS_IN_TexCoord;
    SpriteBase_VSMain(_streams);
    if (false)
    {
        float4 param = _streams.Color_id0;
        _streams.Color_id0 = ColorUtility_ToLinear(param);
    }
    VS_OUT_Color = _streams.Color_id0;
    VS_OUT_ColorAdd = _streams.ColorAdd_id1;
    VS_OUT_Swizzle = _streams.Swizzle_id2;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
    VS_OUT_TexCoord = _streams.TexCoord_id5;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_Color = stage_input.VS_IN_Color;
    VS_IN_ColorAdd = stage_input.VS_IN_ColorAdd;
    VS_IN_Swizzle = stage_input.VS_IN_Swizzle;
    VS_IN_Position = stage_input.VS_IN_Position;
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_Color = VS_OUT_Color;
    stage_output.VS_OUT_ColorAdd = VS_OUT_ColorAdd;
    stage_output.VS_OUT_Swizzle = VS_OUT_Swizzle;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
