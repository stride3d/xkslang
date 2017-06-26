struct PS_STREAMS
{
    int svar_id0;
};

static int PS_OUT_svar;

struct SPIRV_Cross_Output
{
    int PS_OUT_svar : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = { 0 };
    int res = 0;
    res++;
    _streams.svar_id0 = res;
    res++;
    _streams.svar_id0 = res;
    int i = res;
    PS_OUT_svar = _streams.svar_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_svar = PS_OUT_svar;
    return stage_output;
}
