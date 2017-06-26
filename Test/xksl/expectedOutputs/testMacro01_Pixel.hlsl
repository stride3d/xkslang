struct PS_STREAMS
{
    int tabStream01_id0[2];
    int tabStream02_id1[2];
};

static int PS_IN_tabStream01[2];
static int PS_OUT_tabStream02[2];

struct SPIRV_Cross_Input
{
    int PS_IN_tabStream01[2] : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    int PS_OUT_tabStream02[2] : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = { { 0, 0 }, { 0, 0 } };
    _streams.tabStream01_id0 = PS_IN_tabStream01;
    for (int i = 0; i < 2; i++)
    {
        _streams.tabStream02_id1[i] = _streams.tabStream01_id0[i];
    }
    PS_OUT_tabStream02 = _streams.tabStream02_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_tabStream01 = stage_input.PS_IN_tabStream01;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_tabStream02 = PS_OUT_tabStream02;
    return stage_output;
}
