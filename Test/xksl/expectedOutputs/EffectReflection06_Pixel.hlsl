struct PS_STREAMS
{
    float4 aStream_id0;
    float4 ColorTarget_id1;
};

SamplerState ShaderMain_Sampler0;
Texture2D<float4> ShaderMain_Texture0;

static float4 PS_IN_aStream;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_aStream : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.aStream_id0 = PS_IN_aStream;
    _streams.ColorTarget_id1 = ShaderMain_Texture0.Sample(ShaderMain_Sampler0, float2(0.5f, 0.5f)) + _streams.aStream_id0;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_aStream = stage_input.PS_IN_aStream;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
