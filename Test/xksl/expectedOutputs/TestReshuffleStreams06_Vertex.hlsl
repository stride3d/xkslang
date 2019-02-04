struct ShaderMain_Toto
{
    float2 toto1;
    float4 toto2;
    int toto3[2];
};

struct VS_STREAMS
{
    double aDouble_id0;
    int aBool_id1;
    float4 aF4Tab_id2[4];
    float2 aF2_id3;
    float2 aF2b_id4;
    float aFloat_id5;
    ShaderMain_Toto aToto1_id6;
    ShaderMain_Toto aTotoTab_id7[2];
    float4x4 aMat44_id8;
};

static const float4 _63[4] = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };
static const int _65[2] = { 0, 0 };
static const ShaderMain_Toto _66 = { 0.0f.xx, 0.0f.xxxx, { 0, 0 } };
static const ShaderMain_Toto _67[2] = { { 0.0f.xx, 0.0f.xxxx, { 0, 0 } }, { 0.0f.xx, 0.0f.xxxx, { 0, 0 } } };
static const VS_STREAMS _86 = { 0.0, 0, { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx }, 0.0f.xx, 0.0f.xx, 0.0f, { 0.0f.xx, 0.0f.xxxx, { 0, 0 } }, { { 0.0f.xx, 0.0f.xxxx, { 0, 0 } }, { 0.0f.xx, 0.0f.xxxx, { 0, 0 } } }, float4x4(0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx) };

static double VS_IN_ADOUBLE;
static int VS_IN_ABOOL;
static float4 VS_IN_AF4Tab[4];
static float2 VS_IN_AF2b;
static float VS_IN_AF;
static ShaderMain_Toto VS_IN_ATOTO;
static ShaderMain_Toto VS_IN_ATOTOTAB[2];
static float4x4 VS_IN_AMAT44;
static double VS_OUT_aDouble;
static int VS_OUT_aBool;
static float4 VS_OUT_aF4Tab[4];
static float2 VS_OUT_aF2;
static ShaderMain_Toto VS_OUT_aToto1;
static ShaderMain_Toto VS_OUT_aTotoTab[2];
static float4x4 VS_OUT_aMat44;

struct SPIRV_Cross_Input
{
    int VS_IN_ABOOL : ABOOL;
    double VS_IN_ADOUBLE : ADOUBLE;
    float VS_IN_AF : AF;
    float2 VS_IN_AF2b : AF2b;
    float4 VS_IN_AF4Tab[4] : AF4Tab;
    float4x4 VS_IN_AMAT44 : AMAT44;
    ShaderMain_Toto VS_IN_ATOTO : ATOTO;
    ShaderMain_Toto VS_IN_ATOTOTAB[2] : ATOTOTAB;
};

struct SPIRV_Cross_Output
{
    int VS_OUT_aBool : ABOOL;
    double VS_OUT_aDouble : ADOUBLE;
    float2 VS_OUT_aF2 : AF2;
    float4 VS_OUT_aF4Tab[4] : AF4Tab;
    float4x4 VS_OUT_aMat44 : AMAT44;
    ShaderMain_Toto VS_OUT_aToto1 : ATOTO;
    ShaderMain_Toto VS_OUT_aTotoTab[2] : ATOTOTAB;
};

void vert_main()
{
    VS_STREAMS _streams = _86;
    _streams.aDouble_id0 = VS_IN_ADOUBLE;
    _streams.aBool_id1 = VS_IN_ABOOL;
    _streams.aF4Tab_id2 = VS_IN_AF4Tab;
    _streams.aF2b_id4 = VS_IN_AF2b;
    _streams.aFloat_id5 = VS_IN_AF;
    _streams.aToto1_id6 = VS_IN_ATOTO;
    _streams.aTotoTab_id7 = VS_IN_ATOTOTAB;
    _streams.aMat44_id8 = VS_IN_AMAT44;
    _streams.aF2_id3 = float2(0.0f, 1.0f + _streams.aFloat_id5) + _streams.aF2b_id4;
    VS_OUT_aDouble = _streams.aDouble_id0;
    VS_OUT_aBool = _streams.aBool_id1;
    VS_OUT_aF4Tab = _streams.aF4Tab_id2;
    VS_OUT_aF2 = _streams.aF2_id3;
    VS_OUT_aToto1 = _streams.aToto1_id6;
    VS_OUT_aTotoTab = _streams.aTotoTab_id7;
    VS_OUT_aMat44 = _streams.aMat44_id8;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_ADOUBLE = stage_input.VS_IN_ADOUBLE;
    VS_IN_ABOOL = stage_input.VS_IN_ABOOL;
    VS_IN_AF4Tab = stage_input.VS_IN_AF4Tab;
    VS_IN_AF2b = stage_input.VS_IN_AF2b;
    VS_IN_AF = stage_input.VS_IN_AF;
    VS_IN_ATOTO = stage_input.VS_IN_ATOTO;
    VS_IN_ATOTOTAB = stage_input.VS_IN_ATOTOTAB;
    VS_IN_AMAT44[0] = stage_input.VS_IN_AMAT44_0;
    VS_IN_AMAT44[1] = stage_input.VS_IN_AMAT44_1;
    VS_IN_AMAT44[2] = stage_input.VS_IN_AMAT44_2;
    VS_IN_AMAT44[3] = stage_input.VS_IN_AMAT44_3;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aDouble = VS_OUT_aDouble;
    stage_output.VS_OUT_aBool = VS_OUT_aBool;
    stage_output.VS_OUT_aF4Tab = VS_OUT_aF4Tab;
    stage_output.VS_OUT_aF2 = VS_OUT_aF2;
    stage_output.VS_OUT_aToto1 = VS_OUT_aToto1;
    stage_output.VS_OUT_aTotoTab = VS_OUT_aTotoTab;
    stage_output.VS_OUT_aMat44 = VS_OUT_aMat44;
    return stage_output;
}
