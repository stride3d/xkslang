struct PS_STREAMS
{
    float2 aStream_id0;
};

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

static float2 PS_IN_aStream;

struct SPIRV_Cross_Input
{
    float2 PS_IN_aStream : ASTREAM;
};

void frag_main()
{
    PS_STREAMS _streams = { float2(0.0f, 0.0f) };
    _streams.aStream_id0 = PS_IN_aStream;
    float2 uv2 = _streams.aStream_id0;
    float4 noSampler = ShaderMain_Texture0.Sample(ShaderMain_Sampler, uv2);
    float4 pointSampler = ShaderMain_Texture0.Sample(ShaderMain_PointSampler, uv2);
    float4 linearSampler = ShaderMain_Texture0.Sample(ShaderMain_LinearSampler, uv2);
    float4 linearBorderSampler = ShaderMain_Texture0.Sample(ShaderMain_LinearBorderSampler, uv2);
    float _42 = ShaderMain_Texture0.SampleCmp(ShaderMain_LinearClampCompareLessEqualSampler, uv2, uv2.y);
    float4 linearClampCompareLessEqualSampler = float4(_42, _42, _42, _42);
    float4 anisotropicSampler = ShaderMain_Texture0.Sample(ShaderMain_AnisotropicSampler, uv2);
    float4 anisotropicRepeatSampler = ShaderMain_Texture0.Sample(ShaderMain_AnisotropicRepeatSampler, uv2);
    float4 pointRepeatSampler = ShaderMain_Texture0.Sample(ShaderMain_PointRepeatSampler, uv2);
    float4 linearRepeatSampler = ShaderMain_Texture0.Sample(ShaderMain_LinearRepeatSampler, uv2);
    float4 repeatSampler = ShaderMain_Texture0.Sample(ShaderMain_RepeatSampler, uv2);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_aStream = stage_input.PS_IN_aStream;
    frag_main();
}
