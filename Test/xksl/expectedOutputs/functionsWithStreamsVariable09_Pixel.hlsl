struct TestShaderBaseA_Streams
{
    float3 sBaseA;
    int _unused;
};

struct TestShaderBaseB_Streams
{
    float3 sBaseB;
    float3 sBaseA;
    int _unused;
};

struct TestShaderMain_Streams
{
    float3 sMain;
    float3 sBaseB;
    float3 sBaseA;
    int _unused;
};

struct PS_STREAMS
{
    float3 sMain_id0;
    float3 sBaseB_id1;
    float3 sBaseA_id2;
};

static const PS_STREAMS _96 = { 0.0f.xxx, 0.0f.xxx, 0.0f.xxx };

static float3 PS_IN_SMAIN;
static float3 PS_IN_SBASEB;
static float3 PS_IN_SBASEA;

struct SPIRV_Cross_Input
{
    float3 PS_IN_SBASEA : SBASEA;
    float3 PS_IN_SBASEB : SBASEB;
    float3 PS_IN_SMAIN : SMAIN;
};

TestShaderMain_Streams TestShaderMain__getStreams_PS(PS_STREAMS _streams)
{
    TestShaderMain_Streams _86 = { _streams.sMain_id0, _streams.sBaseB_id1, _streams.sBaseA_id2, 0 };
    TestShaderMain_Streams res = _86;
    return res;
}

TestShaderBaseB_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseBStreams(TestShaderMain_Streams s)
{
    TestShaderBaseB_Streams _26 = { s.sBaseB, s.sBaseA, s._unused };
    TestShaderBaseB_Streams r = _26;
    return r;
}

TestShaderBaseA_Streams TestShaderBaseB__ConvertTestShaderBaseBStreamsToTestShaderBaseAStreams(TestShaderBaseB_Streams s)
{
    TestShaderBaseA_Streams _49 = { s.sBaseA, s._unused };
    TestShaderBaseA_Streams r = _49;
    return r;
}

void TestShaderBaseA_Compute(inout TestShaderBaseA_Streams s)
{
    s.sBaseA = 1.0f.xxx;
}

void TestShaderBaseB_Compute(inout TestShaderBaseB_Streams s)
{
    s.sBaseB = 2.0f.xxx;
    TestShaderBaseB_Streams param = s;
    TestShaderBaseA_Streams param_1 = TestShaderBaseB__ConvertTestShaderBaseBStreamsToTestShaderBaseAStreams(param);
    TestShaderBaseA_Compute(param_1);
}

void TestShaderMain_Compute(inout TestShaderMain_Streams s)
{
    s.sMain = 1.0f.xxx;
    TestShaderMain_Streams param = s;
    TestShaderBaseB_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseBStreams(param);
    TestShaderBaseB_Compute(param_1);
}

void frag_main()
{
    PS_STREAMS _streams = _96;
    _streams.sMain_id0 = PS_IN_SMAIN;
    _streams.sBaseB_id1 = PS_IN_SBASEB;
    _streams.sBaseA_id2 = PS_IN_SBASEA;
    TestShaderMain_Streams param = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Compute(param);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SMAIN = stage_input.PS_IN_SMAIN;
    PS_IN_SBASEB = stage_input.PS_IN_SBASEB;
    PS_IN_SBASEA = stage_input.PS_IN_SBASEA;
    frag_main();
}
