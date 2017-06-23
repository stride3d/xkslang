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

static double VS_IN_aDouble;
static int VS_IN_aBool;
static float4 VS_IN_aF4Tab[4];
static float2 VS_IN_aF2b;
static float VS_IN_aFloat;
static ShaderMain_Toto VS_IN_aToto1;
static ShaderMain_Toto VS_IN_aTotoTab[2];
static float4x4 VS_IN_aMat44;
static double VS_OUT_aDouble;
static int VS_OUT_aBool;
static float4 VS_OUT_aF4Tab[4];
static float2 VS_OUT_aF2;
static ShaderMain_Toto VS_OUT_aToto1;
static ShaderMain_Toto VS_OUT_aTotoTab[2];
static float4x4 VS_OUT_aMat44;

struct SPIRV_Cross_Input
{
    double VS_IN_aDouble : ADOUBLE;
    int VS_IN_aBool : ABOOL;
    float4 VS_IN_aF4Tab[4] : AF4Tab;
    float2 VS_IN_aF2b : AF2b;
    float VS_IN_aFloat : AF;
    ShaderMain_Toto VS_IN_aToto1 : ATOTO;
    ShaderMain_Toto VS_IN_aTotoTab[2] : ATOTOTAB;
    float4x4 VS_IN_aMat44 : AMAT44;
};

struct SPIRV_Cross_Output
{
    double VS_OUT_aDouble : ADOUBLE;
    int VS_OUT_aBool : ABOOL;
    float4 VS_OUT_aF4Tab[4] : AF4Tab;
    float2 VS_OUT_aF2 : AF2;
    ShaderMain_Toto VS_OUT_aToto1 : ATOTO;
    ShaderMain_Toto VS_OUT_aTotoTab[2] : ATOTOTAB;
    float4x4 VS_OUT_aMat44 : AMAT44;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0, 0, { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) }, float2(0.0f, 0.0f), float2(0.0f, 0.0f), 0.0f, { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), { 0, 0 } }, { { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), { 0, 0 } }, { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), { 0, 0 } } }, float4x4(float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f)) };
    _streams.aDouble_id0 = VS_IN_aDouble;
    _streams.aBool_id1 = VS_IN_aBool;
    _streams.aF4Tab_id2 = VS_IN_aF4Tab;
    _streams.aF2b_id4 = VS_IN_aF2b;
    _streams.aFloat_id5 = VS_IN_aFloat;
    _streams.aToto1_id6 = VS_IN_aToto1;
    _streams.aTotoTab_id7 = VS_IN_aTotoTab;
    _streams.aMat44_id8 = VS_IN_aMat44;
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
    VS_IN_aDouble = stage_input.VS_IN_aDouble;
    VS_IN_aBool = stage_input.VS_IN_aBool;
    VS_IN_aF4Tab = stage_input.VS_IN_aF4Tab;
    VS_IN_aF2b = stage_input.VS_IN_aF2b;
    VS_IN_aFloat = stage_input.VS_IN_aFloat;
    VS_IN_aToto1 = stage_input.VS_IN_aToto1;
    VS_IN_aTotoTab = stage_input.VS_IN_aTotoTab;
    VS_IN_aMat44[0] = stage_input.VS_IN_aMat44_0;
    VS_IN_aMat44[1] = stage_input.VS_IN_aMat44_1;
    VS_IN_aMat44[2] = stage_input.VS_IN_aMat44_2;
    VS_IN_aMat44[3] = stage_input.VS_IN_aMat44_3;
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
