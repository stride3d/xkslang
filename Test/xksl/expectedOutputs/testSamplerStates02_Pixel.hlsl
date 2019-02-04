struct PS_STREAMS
{
    float2 aStream_id0;
};

static const PS_STREAMS _91 = { 0.0f.xx };

Texture2D<float4> ShaderMain_Texture0;
SamplerState ShaderMain_Sampler;
SamplerState ShaderMain_PointSampler;
SamplerState ShaderMain_LinearSampler;
SamplerState ShaderMain_LinearBorderSampler;
SamplerComparisonState ShaderMain_LinearClampCompareLessEqualSampler;
SamplerState ShaderMain_AnisotropicSampler;
SamplerState ShaderMain_AnisotropicRepeatSampler;
SamplerState ShaderMain_PointRepeatSampler;
SamplerState ShaderMain_LinearRepeatSampler;
SamplerState ShaderMain_RepeatSampler;

static float2 PS_IN_ASTREAM;

struct SPIRV_Cross_Input
{
    float2 PS_IN_ASTREAM : ASTREAM;
};

void frag_main()
{
    PS_STREAMS _streams = _91;
    _streams.aStream_id0 = PS_IN_ASTREAM;
    float2 uv2 = _streams.aStream_id0;
    float4 f_noSampler = ShaderMain_Texture0.Sample(ShaderMain_Sampler, uv2);
    float4 f_pointSampler = ShaderMain_Texture0.Sample(ShaderMain_PointSampler, uv2);
    float4 f_linearSampler = ShaderMain_Texture0.Sample(ShaderMain_LinearSampler, uv2);
    float4 f_linearBorderSampler = ShaderMain_Texture0.Sample(ShaderMain_LinearBorderSampler, uv2);
    float4 f_linearClampCompareLessEqualSampler = ShaderMain_Texture0.SampleCmp(ShaderMain_LinearClampCompareLessEqualSampler, uv2, uv2.y).xxxx;
    float4 f_anisotropicSampler = ShaderMain_Texture0.Sample(ShaderMain_AnisotropicSampler, uv2);
    float4 f_anisotropicRepeatSampler = ShaderMain_Texture0.Sample(ShaderMain_AnisotropicRepeatSampler, uv2);
    float4 f_pointRepeatSampler = ShaderMain_Texture0.Sample(ShaderMain_PointRepeatSampler, uv2);
    float4 f_linearRepeatSampler = ShaderMain_Texture0.Sample(ShaderMain_LinearRepeatSampler, uv2);
    float4 f_repeatSampler = ShaderMain_Texture0.Sample(ShaderMain_RepeatSampler, uv2);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ASTREAM = stage_input.PS_IN_ASTREAM;
    frag_main();
}
