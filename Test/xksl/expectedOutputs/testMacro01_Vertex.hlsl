struct VS_STREAMS
{
    int tabStream01_id0[2];
};

static int VS_OUT_tabStream01[2];

struct SPIRV_Cross_Output
{
    int VS_OUT_tabStream01[2] : TEXCOORD0;
};

void vert_main()
{
    VS_STREAMS _streams = { { 0, 0 } };
    for (int i = 0; i < 2; i++)
    {
        _streams.tabStream01_id0[i] = i;
    }
    VS_OUT_tabStream01 = _streams.tabStream01_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_tabStream01 = VS_OUT_tabStream01;
    return stage_output;
}
