struct PS_STREAMS
{
    float4 outStreamA_id0;
};

static float4 PS_OUT_outStreamA;

struct SPIRV_Cross_Output
{
    float4 PS_OUT_outStreamA : SV_Target0;
};

void ShaderA_computeA(inout PS_STREAMS _streams)
{
    _streams.outStreamA_id0 = 2.0f.xxxx;
}

void ShaderB_computeB(inout PS_STREAMS _streams)
{
    _streams.outStreamA_id0 = 2.0f.xxxx;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx };
    ShaderA_computeA(_streams);
    ShaderB_computeB(_streams);
    PS_OUT_outStreamA = _streams.outStreamA_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_outStreamA = PS_OUT_outStreamA;
    return stage_output;
}
