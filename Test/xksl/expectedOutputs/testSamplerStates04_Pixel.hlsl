struct PS_STREAMS
{
    float2 aStream_id0;
};

static const PS_STREAMS _63 = { 0.0f.xx };

Texture2D<float4> ShaderMain_Texture0;
SamplerState ShaderMain_Sampler01;
SamplerState ShaderMain_Sampler02;
SamplerState ShaderMain_Sampler03;
SamplerState ShaderMain_Sampler04;
SamplerState ShaderMain_Sampler05;
SamplerState ShaderMain_Sampler06;

static float2 PS_IN_ASTREAM;

struct SPIRV_Cross_Input
{
    float2 PS_IN_ASTREAM : ASTREAM;
};

void frag_main()
{
    PS_STREAMS _streams = _63;
    _streams.aStream_id0 = PS_IN_ASTREAM;
    float2 uv2 = _streams.aStream_id0;
    float4 f01 = ShaderMain_Texture0.Sample(ShaderMain_Sampler01, uv2);
    float4 f02 = ShaderMain_Texture0.Sample(ShaderMain_Sampler02, uv2);
    float4 f03 = ShaderMain_Texture0.Sample(ShaderMain_Sampler03, uv2);
    float4 f04 = ShaderMain_Texture0.Sample(ShaderMain_Sampler04, uv2);
    float4 f05 = ShaderMain_Texture0.Sample(ShaderMain_Sampler05, uv2);
    float4 f06 = ShaderMain_Texture0.Sample(ShaderMain_Sampler06, uv2);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ASTREAM = stage_input.PS_IN_ASTREAM;
    frag_main();
}
