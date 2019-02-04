struct PS_STREAMS
{
    float4 outStreamA_id0;
};

static const PS_STREAMS _20 = { 0.0f.xxxx };

static float4 PS_OUT_outStreamA;

struct SPIRV_Cross_Output
{
    float4 PS_OUT_outStreamA : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = _20;
    _streams.outStreamA_id0 = 2.0f.xxxx;
    PS_OUT_outStreamA = _streams.outStreamA_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_outStreamA = PS_OUT_outStreamA;
    return stage_output;
}
