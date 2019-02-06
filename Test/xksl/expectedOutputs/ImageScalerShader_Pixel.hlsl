struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float2 TexCoord_id2;
};

static const PS_STREAMS _65 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx };

cbuffer Globals
{
    float4 ImageScalerShader_Color;
    float ImageScalerShader_IsOnlyChannelRed;
};
Texture2D<float4> Texturing_Texture0;
SamplerState Texturing_Sampler;

static float4 PS_IN_SV_Position;
static float2 PS_IN_TEXCOORD0;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_SV_Position : SV_Position;
    float2 PS_IN_TEXCOORD0 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float4 SpriteBase_Shading(PS_STREAMS _streams)
{
    return Texturing_Texture0.Sample(Texturing_Sampler, _streams.TexCoord_id2);
}

float4 ImageScalerShader_Shading(PS_STREAMS _streams)
{
    float4 color = SpriteBase_Shading(_streams);
    if (ImageScalerShader_IsOnlyChannelRed != 0.0f)
    {
        color = float4(color.xxx, 1.0f);
    }
    return color * ImageScalerShader_Color;
}

void frag_main()
{
    PS_STREAMS _streams = _65;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.TexCoord_id2 = PS_IN_TEXCOORD0;
    _streams.ColorTarget_id1 = ImageScalerShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    PS_IN_TEXCOORD0 = stage_input.PS_IN_TEXCOORD0;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
