struct PS_STREAMS
{
    int aStream2_id0;
    float4 aStream3_id1;
    float4 aStreamBis_id2;
    int aStream1_id3;
    int aStream1_id4;
};

static int PS_IN_aStream2;
static float4 PS_IN_aStream3;
static float4 PS_OUT_aStreamBis;
static int PS_OUT_aStream1;
static int PS_OUT_aStream1_1;

struct SPIRV_Cross_Input
{
    int PS_IN_aStream2 : TEXCOORD0;
    float4 PS_IN_aStream3 : SV_SEMANTIC;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_aStreamBis : SV_OUTPUT;
    int PS_OUT_aStream1 : SV_Target1;
    int PS_OUT_aStream1_1 : SV_Target2;
};

int o0S5C0_Color_Compute(inout PS_STREAMS _streams, int i)
{
    float _33 = float(i);
    _streams.aStreamBis_id2 = float4(_33, _33, _33, _33);
    _streams.aStream1_id3 = i + _streams.aStream2_id0;
    float _44 = float(i);
    return int4(_streams.aStream3_id1 + float4(_44, _44, _44, _44)).x;
}

int o1S5C0_Color_Compute(inout PS_STREAMS _streams, int i)
{
    float _53 = float(i);
    _streams.aStreamBis_id2 = float4(_53, _53, _53, _53);
    _streams.aStream1_id4 = i + _streams.aStream2_id0;
    float _64 = float(i);
    return int4(_streams.aStream3_id1 + float4(_64, _64, _64, _64)).x;
}

void frag_main()
{
    PS_STREAMS _streams = { 0, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0, 0 };
    _streams.aStream2_id0 = PS_IN_aStream2;
    _streams.aStream3_id1 = PS_IN_aStream3;
    int res = 0;
    int param = 1;
    int _69 = o0S5C0_Color_Compute(_streams, param);
    res += _69;
    param = 1;
    int _72 = o1S5C0_Color_Compute(_streams, param);
    res += _72;
    float _8 = float(res);
    _streams.aStreamBis_id2 += float4(_8, _8, _8, _8);
    int i = int4(_streams.aStreamBis_id2).x;
    PS_OUT_aStreamBis = _streams.aStreamBis_id2;
    PS_OUT_aStream1 = _streams.aStream1_id3;
    PS_OUT_aStream1_1 = _streams.aStream1_id4;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_aStream2 = stage_input.PS_IN_aStream2;
    PS_IN_aStream3 = stage_input.PS_IN_aStream3;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_aStreamBis = PS_OUT_aStreamBis;
    stage_output.PS_OUT_aStream1 = PS_OUT_aStream1;
    stage_output.PS_OUT_aStream1_1 = PS_OUT_aStream1_1;
    return stage_output;
}
