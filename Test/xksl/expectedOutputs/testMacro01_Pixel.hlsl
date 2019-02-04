struct PS_STREAMS
{
    int tabStream01_id0[2];
    int tabStream02_id1[2];
};

static const int _32[2] = { 0, 0 };
static const PS_STREAMS _36 = { { 0, 0 }, { 0, 0 } };

static int PS_IN_TABSTREAM01[2];

struct SPIRV_Cross_Input
{
    int PS_IN_TABSTREAM01[2] : TABSTREAM01;
};

void frag_main()
{
    PS_STREAMS _streams = _36;
    _streams.tabStream01_id0 = PS_IN_TABSTREAM01;
    for (int i = 0; i < 2; i++)
    {
        _streams.tabStream02_id1[i] = _streams.tabStream01_id0[i];
    }
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TABSTREAM01 = stage_input.PS_IN_TABSTREAM01;
    frag_main();
}
