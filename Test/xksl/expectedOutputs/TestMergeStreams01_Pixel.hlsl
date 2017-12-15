struct PS_STREAMS
{
    int aStream1_id0;
    int aStream2_id1;
    float4 aStream3_id2;
    float4 aStreamBis_id3;
};

static int PS_IN_aStream2;
static float4 PS_IN_aStream3;
static float4 PS_OUT_aStreamBis;

struct SPIRV_Cross_Input
{
    int PS_IN_aStream2 : ASTREAM2;
    float4 PS_IN_aStream3 : SV_SEMANTIC;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_aStreamBis : SV_Target0;
};

int o0S5C0_Color_Compute(inout PS_STREAMS _streams, int i)
{
    _streams.aStreamBis_id3 = float(i).xxxx;
    _streams.aStream1_id0 = i + _streams.aStream2_id1;
    return int4(_streams.aStream3_id2 + float(i).xxxx).x;
}

int o1S5C0_Color_Compute(inout PS_STREAMS _streams, int i)
{
    _streams.aStreamBis_id3 = float(i).xxxx;
    _streams.aStream1_id0 = i + _streams.aStream2_id1;
    return int4(_streams.aStream3_id2 + float(i).xxxx).x;
}

void frag_main()
{
    PS_STREAMS _streams = { 0, 0, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.aStream2_id1 = PS_IN_aStream2;
    _streams.aStream3_id2 = PS_IN_aStream3;
    int res = 0;
    int param = 1;
    int _69 = o0S5C0_Color_Compute(_streams, param);
    res += _69;
    param = 1;
    int _72 = o1S5C0_Color_Compute(_streams, param);
    res += _72;
    _streams.aStreamBis_id3 += float(res).xxxx;
    int i = int4(_streams.aStreamBis_id3).x;
    PS_OUT_aStreamBis = _streams.aStreamBis_id3;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_aStream2 = stage_input.PS_IN_aStream2;
    PS_IN_aStream3 = stage_input.PS_IN_aStream3;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_aStreamBis = PS_OUT_aStreamBis;
    return stage_output;
}
