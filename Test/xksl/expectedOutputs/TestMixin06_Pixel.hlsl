struct PS_STREAMS
{
    int streamI_id0;
};

static const PS_STREAMS _14 = { 0 };

static int PS_OUT_streamI;

struct SPIRV_Cross_Output
{
    int PS_OUT_streamI : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = _14;
    _streams.streamI_id0 = 0;
    PS_OUT_streamI = _streams.streamI_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_streamI = PS_OUT_streamI;
    return stage_output;
}
