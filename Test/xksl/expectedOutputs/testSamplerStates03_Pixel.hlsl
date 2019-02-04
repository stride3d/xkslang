struct PS_STREAMS
{
    float2 aStream_id0;
};

static const PS_STREAMS _33 = { 0.0f.xx };

SamplerState ShaderMain_Sampler;
Texture2D<float4> ShaderMain_Texture0;

static float2 PS_IN_ASTREAM;

struct SPIRV_Cross_Input
{
    float2 PS_IN_ASTREAM : ASTREAM;
};

void frag_main()
{
    PS_STREAMS _streams = _33;
    _streams.aStream_id0 = PS_IN_ASTREAM;
    float2 uv2 = _streams.aStream_id0;
    float4 color = ShaderMain_Texture0.Sample(ShaderMain_Sampler, uv2);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ASTREAM = stage_input.PS_IN_ASTREAM;
    frag_main();
}
