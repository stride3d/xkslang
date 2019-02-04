struct TestIComposition_Streams
{
    float3 sComp;
    int _unused;
};

struct TestShaderMain_Streams
{
    float3 sMain;
    float3 sBase;
    int _unused;
};

struct VS_STREAMS
{
    float3 sComp_id0;
    float3 sMain_id1;
    float3 sBase_id2;
};

static const VS_STREAMS _90 = { 0.0f.xxx, 0.0f.xxx, 0.0f.xxx };

static float3 VS_IN_SMAIN;
static float3 VS_IN_SBASE;
static float3 VS_OUT_sMain;
static float3 VS_OUT_sBase;

struct SPIRV_Cross_Input
{
    float3 VS_IN_SBASE : SBASE;
    float3 VS_IN_SMAIN : SMAIN;
};

struct SPIRV_Cross_Output
{
    float3 VS_OUT_sBase : SBASE;
    float3 VS_OUT_sMain : SMAIN;
};

TestShaderMain_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestShaderMain_Streams _27 = { _streams.sMain_id1, _streams.sBase_id2, 0 };
    TestShaderMain_Streams res = _27;
    return res;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(TestShaderMain_Streams s)
{
    TestIComposition_Streams _37 = { s.sBase, s._unused };
    TestIComposition_Streams r = _37;
    return r;
}

void TestShaderBase_Compute(inout TestIComposition_Streams s)
{
    s.sComp = 1.0f.xxx;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(TestShaderMain_Streams s)
{
    TestIComposition_Streams _45 = { 0.0f.xxx, s._unused };
    TestIComposition_Streams r = _45;
    return r;
}

void o0S5C0_TestIComposition_ComputeComp(inout VS_STREAMS _streams, TestIComposition_Streams s)
{
    _streams.sComp_id0 = s.sComp;
}

void TestShaderMain_Compute(inout VS_STREAMS _streams, inout TestShaderMain_Streams s)
{
    s.sMain = 2.0f.xxx;
    TestShaderMain_Streams param = s;
    TestIComposition_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param);
    TestShaderBase_Compute(param_1);
    TestShaderMain_Streams backup = TestShaderMain__getStreams(_streams);
    TestShaderMain_Streams param_2 = backup;
    TestIComposition_Streams param_3 = TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(param_2);
    o0S5C0_TestIComposition_ComputeComp(_streams, param_3);
}

void vert_main()
{
    VS_STREAMS _streams = _90;
    _streams.sMain_id1 = VS_IN_SMAIN;
    _streams.sBase_id2 = VS_IN_SBASE;
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(_streams, param);
    VS_OUT_sMain = _streams.sMain_id1;
    VS_OUT_sBase = _streams.sBase_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_SMAIN = stage_input.VS_IN_SMAIN;
    VS_IN_SBASE = stage_input.VS_IN_SBASE;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_sMain = VS_OUT_sMain;
    stage_output.VS_OUT_sBase = VS_OUT_sBase;
    return stage_output;
}
