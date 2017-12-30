struct TestIComposition_Streams
{
    float3 sComp;
    int _unused;
};

struct TestShaderMain_Streams
{
    float3 sMain;
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

TestShaderMain_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestShaderMain_Streams res = { _streams.sMain_id1, 0 };
    return res;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(TestShaderMain_Streams s)
{
    TestIComposition_Streams r = { float3(0.0f, 0.0f, 0.0f), s._unused };
    return r;
}

void o0S5C0_TestIComposition_ComputeComp(inout VS_STREAMS _streams, TestShaderMain_Streams s)
{
    _streams.sComp_id0 = s.sMain;
}

void TestShaderMain_Compute(inout VS_STREAMS _streams, inout TestShaderMain_Streams s)
{
    s.sMain = float3(2.0f, 2.0f, 2.0f);
    TestShaderMain_Streams backup = TestShaderMain__getStreams(_streams);
    TestShaderMain_Streams param = backup;
    TestIComposition_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(param);
    o0S5C0_TestIComposition_ComputeComp(_streams, param_1);
}

void vert_main()
{
    VS_STREAMS _streams = { float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f) };
    _streams.sMain_id1 = VS_IN_sMain;
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
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
