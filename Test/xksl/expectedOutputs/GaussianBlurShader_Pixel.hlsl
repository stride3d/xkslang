struct PS_STREAMS
{
    float2 TexCoord_id0;
    float4 ShadingPosition_id1;
    float4 ColorTarget_id2;
};

static const PS_STREAMS _117 = { 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer Globals
{
    float2 GaussianBlurShader_OffsetsWeights[3];
    float2 Texturing_Texture0TexelSize;
};
Texture2D<float4> Texturing_Texture0;
SamplerState Texturing_LinearSampler;

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

float4 GaussianBlurShader_3_false__Shading(PS_STREAMS _streams)
{
    float2 direction = float2(1.0f, 0.0f) * Texturing_Texture0TexelSize;
    float3 value = Texturing_Texture0.Sample(Texturing_LinearSampler, _streams.TexCoord_id0).xyz * GaussianBlurShader_OffsetsWeights[0].y;
    for (int i = 1; i < 3; i++)
    {
        value += (Texturing_Texture0.Sample(Texturing_LinearSampler, _streams.TexCoord_id0 - (direction * GaussianBlurShader_OffsetsWeights[i].x)).xyz * GaussianBlurShader_OffsetsWeights[i].y);
        value += (Texturing_Texture0.Sample(Texturing_LinearSampler, _streams.TexCoord_id0 + (direction * GaussianBlurShader_OffsetsWeights[i].x)).xyz * GaussianBlurShader_OffsetsWeights[i].y);
    }
    return float4(value, 1.0f);
}

void frag_main()
{
    PS_STREAMS _streams = _117;
    _streams.TexCoord_id0 = PS_IN_TEXCOORD0;
    _streams.ShadingPosition_id1 = PS_IN_SV_Position;
    _streams.ColorTarget_id2 = GaussianBlurShader_3_false__Shading(_streams);
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
