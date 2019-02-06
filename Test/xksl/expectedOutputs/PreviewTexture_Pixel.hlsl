struct PS_STREAMS
{
    float4 Color_id0;
    float4 ColorAdd_id1;
    float Swizzle_id2;
    float4 ShadingPosition_id3;
    float4 ColorTarget_id4;
    float2 TexCoord_id5;
};

static const PS_STREAMS _128 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx };

Texture2D<float4> Texturing_Texture0;
SamplerState Texturing_Sampler;

static float4 PS_IN_COLOR;
static float4 PS_IN_COLOR1;
static float PS_IN_BATCH_SWIZZLE;
static float4 PS_IN_SV_Position;
static float2 PS_IN_TEXCOORD0;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float PS_IN_BATCH_SWIZZLE : BATCH_SWIZZLE;
    float4 PS_IN_COLOR : COLOR;
    float4 PS_IN_COLOR1 : COLOR1;
    float4 PS_IN_SV_Position : SV_Position;
    float2 PS_IN_TEXCOORD0 : TEXCOORD0;
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
    float4 _15 = SpriteBase_Shading(_streams).xxxx;
    float4 _16 = SpriteBase_Shading(_streams);
    bool4 _17 = (abs(_streams.Swizzle_id2 - 1.0f) <= 0.100000001490116119384765625f).xxxx;
    float4 swizzleColor = float4(_17.x ? _15.x : _16.x, _17.y ? _15.y : _16.y, _17.z ? _15.z : _16.z, _17.w ? _15.w : _16.w);
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
    PS_STREAMS _streams = _128;
    _streams.Color_id0 = PS_IN_COLOR;
    _streams.ColorAdd_id1 = PS_IN_COLOR1;
    _streams.Swizzle_id2 = PS_IN_BATCH_SWIZZLE;
    _streams.ShadingPosition_id3 = PS_IN_SV_Position;
    _streams.TexCoord_id5 = PS_IN_TEXCOORD0;
    _streams.ColorTarget_id4 = SpriteBatchShader_false__Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id4;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_COLOR = stage_input.PS_IN_COLOR;
    PS_IN_COLOR1 = stage_input.PS_IN_COLOR1;
    PS_IN_BATCH_SWIZZLE = stage_input.PS_IN_BATCH_SWIZZLE;
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    PS_IN_TEXCOORD0 = stage_input.PS_IN_TEXCOORD0;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
