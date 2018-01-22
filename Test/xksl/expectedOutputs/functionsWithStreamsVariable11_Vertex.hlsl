struct TestIComposition_Streams
{
    float3 sComp;
    int _unused;
};

struct VS_STREAMS
{
    float3 sComp_id0;
    float3 sMain_id1;
};

static float3 VS_IN_sMain;
static float3 VS_OUT_sComp;

struct SPIRV_Cross_Input
{
    float3 VS_IN_sMain : SMAIN;
};

struct SPIRV_Cross_Output
{
    float3 VS_OUT_sComp : SCOMP;
};

TestIComposition_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestIComposition_Streams res = TestIComposition_Streams{ _streams.sMain_id1, 0 };
    return res;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(TestIComposition_Streams s)
{
    TestIComposition_Streams r = TestIComposition_Streams{ 0.0f.xxx, s._unused };
    return r;
}

void o0S5C0_TestIComposition_ComputeComp(inout VS_STREAMS _streams, TestIComposition_Streams s)
{
    _streams.sComp_id0 = s.sComp;
}

void TestShaderMain_Compute(inout VS_STREAMS _streams, inout TestIComposition_Streams s)
{
    s.sComp = 2.0f.xxx;
    TestIComposition_Streams backup = TestShaderMain__getStreams(_streams);
    TestIComposition_Streams param = backup;
    TestIComposition_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(param);
    o0S5C0_TestIComposition_ComputeComp(_streams, param_1);
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxx, 0.0f.xxx };
    _streams.sMain_id1 = VS_IN_sMain;
    TestIComposition_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(_streams, param);
    VS_OUT_sComp = _streams.sComp_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_sMain = stage_input.VS_IN_sMain;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_sComp = VS_OUT_sComp;
    return stage_output;
}
