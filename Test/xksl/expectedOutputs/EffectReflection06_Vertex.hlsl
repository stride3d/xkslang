struct VS_STREAMS
{
    float4 aStream_id0;
};

SamplerState ShaderMain_Sampler0;
Texture2D<float4> ShaderMain_Texture0;

static float4 VS_OUT_aStream;

struct SPIRV_Cross_Output
{
    float4 VS_OUT_aStream : ASTREAM;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx };
    _streams.aStream_id0 = ShaderMain_Texture0.SampleLevel(ShaderMain_Sampler0, 0.4000000059604644775390625f.xx, 0.0f);
    VS_OUT_aStream = _streams.aStream_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream = VS_OUT_aStream;
    return stage_output;
}
