struct ShaderMain_Toto
{
    float2 toto1;
    float4 toto2;
    int toto3[2];
};

struct PS_STREAMS
{
    double aDouble_id0;
    int aBool_id1;
    float4 aF4Tab_id2[4];
    float2 aF2_id3;
    float4 aF4_id4;
    ShaderMain_Toto aToto1_id5;
    ShaderMain_Toto aTotoTab_id6[2];
    float4x4 aMat44_id7;
};

static double PS_IN_aDouble;
static int PS_IN_aBool;
static float4 PS_IN_aF4Tab[4];
static float2 PS_IN_aF2;
static ShaderMain_Toto PS_IN_aToto1;
static ShaderMain_Toto PS_IN_aTotoTab[2];
static float4x4 PS_IN_aMat44;
static float4 PS_OUT_aF4;

struct SPIRV_Cross_Input
{
    double PS_IN_aDouble : ADOUBLE;
    int PS_IN_aBool : ABOOL;
    float4 PS_IN_aF4Tab[4] : AF4Tab;
    float2 PS_IN_aF2 : AF2;
    ShaderMain_Toto PS_IN_aToto1 : ATOTO;
    ShaderMain_Toto PS_IN_aTotoTab[2] : ATOTOTAB;
    float4x4 PS_IN_aMat44 : AMAT44;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_aF4 : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = { 0.0, 0, { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) }, float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), { 0, 0 } }, { { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), { 0, 0 } }, { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), { 0, 0 } } }, float4x4(float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f)) };
    _streams.aDouble_id0 = PS_IN_aDouble;
    _streams.aBool_id1 = PS_IN_aBool;
    _streams.aF4Tab_id2 = PS_IN_aF4Tab;
    _streams.aF2_id3 = PS_IN_aF2;
    _streams.aToto1_id5 = PS_IN_aToto1;
    _streams.aTotoTab_id6 = PS_IN_aTotoTab;
    _streams.aMat44_id7 = PS_IN_aMat44;
    double dd = 0.0;
    double d = _streams.aDouble_id0;
    float4 aaa = _streams.aF4Tab_id2[0];
    ShaderMain_Toto toto = _streams.aToto1_id5;
    ShaderMain_Toto toto2 = _streams.aTotoTab_id6[1];
    float4x4 aMat = _streams.aMat44_id7;
    bool b = _streams.aBool_id1 != int(0u);
    _streams.aF4_id4 = float4(_streams.aF2_id3, 0.0f, 1.0f);
    PS_OUT_aF4 = _streams.aF4_id4;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_aDouble = stage_input.PS_IN_aDouble;
    PS_IN_aBool = stage_input.PS_IN_aBool;
    PS_IN_aF4Tab = stage_input.PS_IN_aF4Tab;
    PS_IN_aF2 = stage_input.PS_IN_aF2;
    PS_IN_aToto1 = stage_input.PS_IN_aToto1;
    PS_IN_aTotoTab = stage_input.PS_IN_aTotoTab;
    PS_IN_aMat44 = stage_input.PS_IN_aMat44;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_aF4 = PS_OUT_aF4;
    return stage_output;
}
