struct PS_STREAMS
{
    float2 TexCoord_id0;
    float4 ShadingPosition_id1;
    float4 ColorTarget_id2;
};

cbuffer Globals
{
    float2 Effect_Center;
    float Effect_Frequency;
    float Effect_Phase;
    float Effect_Spread;
    float Effect_Amplitude;
    float Effect_InvAspectRatio;
};
Texture2D<float4> Texturing_Texture0;
SamplerState Texturing_Sampler;

static float2 PS_IN_TexCoord;
static float4 PS_IN_ShadingPosition;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float2 PS_IN_TexCoord : TEXCOORD0;
    float4 PS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float4 Effect_Shading(PS_STREAMS _streams)
{
    float2 toPixel = (_streams.TexCoord_id0 - Effect_Center) * float2(1.0f, Effect_InvAspectRatio);
    float distance = length(toPixel);
    float2 direction = normalize(toPixel);
    float2 wave;
    wave.x = sin((Effect_Frequency * distance) + Effect_Phase);
    wave.y = cos((Effect_Frequency * distance) + Effect_Phase);
    float falloff = clamp(1.0f - distance, 0.0f, 1.0f);
    falloff = pow(falloff, 1.0f / Effect_Spread);
    float2 uv2 = _streams.TexCoord_id0 + (direction * ((wave.x * falloff) * Effect_Amplitude));
    float lighting = lerp(1.0f, 1.0f + ((wave.x * falloff) * 0.20000000298023223876953125f), clamp(Effect_Amplitude / 0.014999999664723873138427734375f, 0.0f, 1.0f));
    float4 color = Texturing_Texture0.Sample(Texturing_Sampler, uv2);
    float3 _82 = color.xyz * lighting;
    color = float4(_82.x, _82.y, _82.z, color.w);
    return color;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx };
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id1 = PS_IN_ShadingPosition;
    _streams.ColorTarget_id2 = Effect_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
