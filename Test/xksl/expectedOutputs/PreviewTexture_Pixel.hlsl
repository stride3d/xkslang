struct PS_STREAMS
{
    float4 Color_id0;
    float4 ColorAdd_id1;
    float Swizzle_id2;
    float4 ShadingPosition_id3;
    float4 ColorTarget_id4;
    float2 TexCoord_id5;
};

Texture2D<float4> Texturing_Texture0;
SamplerState Texturing_Sampler;

static float4 PS_IN_Color;
static float4 PS_IN_ColorAdd;
static float PS_IN_Swizzle;
static float4 PS_IN_ShadingPosition;
static float2 PS_IN_TexCoord;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_Color : COLOR;
    float4 PS_IN_ColorAdd : COLOR1;
    float PS_IN_Swizzle : BATCH_SWIZZLE;
    float4 PS_IN_ShadingPosition : SV_Position;
    float2 PS_IN_TexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float4 SpriteBase_Shading(PS_STREAMS _streams)
{
    return Texturing_Texture0.Sample(Texturing_Sampler, _streams.TexCoord_id5);
}

float4 SpriteBatchShader_false__Shading(PS_STREAMS _streams)
{
    float4 swizzleColor;
    float4 _5;
    if (abs(_streams.Swizzle_id2 - 1.0f) <= 0.100000001490116119384765625f)
    {
        _5 = SpriteBase_Shading(_streams).xxxx;
    }
    else
    {
        _5 = SpriteBase_Shading(_streams);
    }
    swizzleColor = _5;
    if (abs(_streams.Swizzle_id2 - 2.0f) <= 0.100000001490116119384765625f)
    {
        float nX = (swizzleColor.x * 2.0f) - 1.0f;
        float nY = (swizzleColor.y * 2.0f) - 1.0f;
        swizzleColor.w = 1.0f;
        float nZ = 1.0f - sqrt(clamp((nX * nX) + (nY * nY), 0.0f, 1.0f));
        swizzleColor.z = (nZ * 0.5f) + 0.5f;
    }
    if (abs(_streams.Swizzle_id2 - 3.0f) <= 0.100000001490116119384765625f)
    {
        swizzleColor = float4(swizzleColor.x, swizzleColor.xx.x, swizzleColor.xx.y, swizzleColor.w);
        swizzleColor.w = 1.0f;
    }
    float4 finalColor = (swizzleColor * _streams.Color_id0) + _streams.ColorAdd_id1;
    return finalColor;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx };
    _streams.Color_id0 = PS_IN_Color;
    _streams.ColorAdd_id1 = PS_IN_ColorAdd;
    _streams.Swizzle_id2 = PS_IN_Swizzle;
    _streams.ShadingPosition_id3 = PS_IN_ShadingPosition;
    _streams.TexCoord_id5 = PS_IN_TexCoord;
    _streams.ColorTarget_id4 = SpriteBatchShader_false__Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id4;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_Color = stage_input.PS_IN_Color;
    PS_IN_ColorAdd = stage_input.PS_IN_ColorAdd;
    PS_IN_Swizzle = stage_input.PS_IN_Swizzle;
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
