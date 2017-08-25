struct PS_STREAMS
{
    float2 aStream_id0;
};

Texture2D<float4> ShaderMain_Texture0;
SamplerState ShaderMain_SamplerDefault;

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
    float4 color = ShaderMain_Texture0.Sample(ShaderMain_SamplerDefault, uv2);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_aStream = stage_input.PS_IN_aStream;
    frag_main();
}
