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

static const float4 _76[4] = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };
static const int _78[2] = { 0, 0 };
static const ShaderMain_Toto _79 = { 0.0f.xx, 0.0f.xxxx, { 0, 0 } };
static const ShaderMain_Toto _80[2] = { { 0.0f.xx, 0.0f.xxxx, { 0, 0 } }, { 0.0f.xx, 0.0f.xxxx, { 0, 0 } } };
static const PS_STREAMS _92 = { 0.0, 0, { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx }, 0.0f.xx, 0.0f.xxxx, { 0.0f.xx, 0.0f.xxxx, { 0, 0 } }, { { 0.0f.xx, 0.0f.xxxx, { 0, 0 } }, { 0.0f.xx, 0.0f.xxxx, { 0, 0 } } }, float4x4(0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx) };

static double PS_IN_ADOUBLE;
static int PS_IN_ABOOL;
static float4 PS_IN_AF4Tab[4];
static float2 PS_IN_AF2;
static ShaderMain_Toto PS_IN_ATOTO;
static ShaderMain_Toto PS_IN_ATOTOTAB[2];
static float4x4 PS_IN_AMAT44;
static float4 PS_OUT_aF4;

struct SPIRV_Cross_Input
{
    int PS_IN_ABOOL : ABOOL;
    double PS_IN_ADOUBLE : ADOUBLE;
    float2 PS_IN_AF2 : AF2;
    float4 PS_IN_AF4Tab[4] : AF4Tab;
    float4x4 PS_IN_AMAT44 : AMAT44;
    ShaderMain_Toto PS_IN_ATOTO : ATOTO;
    ShaderMain_Toto PS_IN_ATOTOTAB[2] : ATOTOTAB;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_aF4 : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = _92;
    _streams.aDouble_id0 = PS_IN_ADOUBLE;
    _streams.aBool_id1 = PS_IN_ABOOL;
    _streams.aF4Tab_id2 = PS_IN_AF4Tab;
    _streams.aF2_id3 = PS_IN_AF2;
    _streams.aToto1_id5 = PS_IN_ATOTO;
    _streams.aTotoTab_id6 = PS_IN_ATOTOTAB;
    _streams.aMat44_id7 = PS_IN_AMAT44;
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
    PS_IN_ADOUBLE = stage_input.PS_IN_ADOUBLE;
    PS_IN_ABOOL = stage_input.PS_IN_ABOOL;
    PS_IN_AF4Tab = stage_input.PS_IN_AF4Tab;
    PS_IN_AF2 = stage_input.PS_IN_AF2;
    PS_IN_ATOTO = stage_input.PS_IN_ATOTO;
    PS_IN_ATOTOTAB = stage_input.PS_IN_ATOTOTAB;
    PS_IN_AMAT44 = stage_input.PS_IN_AMAT44;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_aF4 = PS_OUT_aF4;
    return stage_output;
}
