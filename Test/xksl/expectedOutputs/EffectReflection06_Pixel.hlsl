struct PS_STREAMS
{
    float4 aStream_id0;
    float4 ColorTarget_id1;
};

static const PS_STREAMS _36 = { 0.0f.xxxx, 0.0f.xxxx };

SamplerState ShaderMain_Sampler0;
Texture2D<float4> ShaderMain_Texture0;

static float4 PS_IN_ASTREAM;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ASTREAM : ASTREAM;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = _36;
    _streams.aStream_id0 = PS_IN_ASTREAM;
    _streams.ColorTarget_id1 = ShaderMain_Texture0.Sample(ShaderMain_Sampler0, 0.5f.xx) + _streams.aStream_id0;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ASTREAM = stage_input.PS_IN_ASTREAM;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
