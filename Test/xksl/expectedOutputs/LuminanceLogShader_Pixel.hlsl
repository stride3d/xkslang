struct PS_STREAMS
{
    float2 TexCoord_id0;
    float4 ShadingPosition_id1;
    float4 ColorTarget_id2;
};

static const PS_STREAMS _74 = { 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx };

Texture2D<float4> Texturing_Texture0;
SamplerState Texturing_PointSampler;

static float2 PS_IN_TEXCOORD0;
static float4 PS_IN_SV_Position;
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

float LuminanceUtils_Luma(float3 color)
{
    return max(dot(color, float3(0.2989999949932098388671875f, 0.58700001239776611328125f, 0.114000000059604644775390625f)), 9.9999997473787516355514526367188e-05f);
}

float LuminanceLogShader_GetLuminance(float3 color)
{
    float3 param = color;
    return LuminanceUtils_Luma(param);
}

float4 LuminanceLogShader_Shading(PS_STREAMS _streams)
{
    float3 color = Texturing_Texture0.Sample(Texturing_PointSampler, _streams.TexCoord_id0).xyz;
    float3 param = color;
    float lum = max(0.001000000047497451305389404296875f, LuminanceLogShader_GetLuminance(param));
    return float4(log2(lum), 1.0f, 1.0f, 1.0f);
}

void frag_main()
{
    PS_STREAMS _streams = _74;
    _streams.TexCoord_id0 = PS_IN_TEXCOORD0;
    _streams.ShadingPosition_id1 = PS_IN_SV_Position;
    _streams.ColorTarget_id2 = LuminanceLogShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TEXCOORD0 = stage_input.PS_IN_TEXCOORD0;
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
