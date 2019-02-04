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

struct PS_STREAMS
{
    float3 sComp_id0;
    float3 sMain_id1;
    float3 sBase_id2;
};

static const PS_STREAMS _87 = { 0.0f.xxx, 0.0f.xxx, 0.0f.xxx };

static float3 PS_IN_SMAIN;
static float3 PS_IN_SBASE;

struct SPIRV_Cross_Input
{
    float3 PS_IN_SBASE : SBASE;
    float3 PS_IN_SMAIN : SMAIN;
};

TestShaderMain_Streams TestShaderMain__getStreams_PS(PS_STREAMS _streams)
{
    TestShaderMain_Streams _58 = { _streams.sMain_id1, _streams.sBase_id2, 0 };
    TestShaderMain_Streams res = _58;
    return res;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(TestShaderMain_Streams s)
{
    TestIComposition_Streams _15 = { s.sBase, s._unused };
    TestIComposition_Streams r = _15;
    return r;
}

void TestShaderBase_Compute(inout TestIComposition_Streams s)
{
    s.sComp = 1.0f.xxx;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(TestShaderMain_Streams s)
{
    TestIComposition_Streams _23 = { 0.0f.xxx, s._unused };
    TestIComposition_Streams r = _23;
    return r;
}

void o0S5C0_TestIComposition_ComputeComp_PS(inout PS_STREAMS _streams, TestIComposition_Streams s)
{
    _streams.sComp_id0 = s.sComp;
}

void TestShaderMain_Compute_PS(inout PS_STREAMS _streams, inout TestShaderMain_Streams s)
{
    s.sMain = 2.0f.xxx;
    TestShaderMain_Streams param = s;
    TestIComposition_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param);
    TestShaderBase_Compute(param_1);
    TestShaderMain_Streams backup = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Streams param_2 = backup;
    TestIComposition_Streams param_3 = TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(param_2);
    o0S5C0_TestIComposition_ComputeComp_PS(_streams, param_3);
}

void frag_main()
{
    PS_STREAMS _streams = _87;
    _streams.sMain_id1 = PS_IN_SMAIN;
    _streams.sBase_id2 = PS_IN_SBASE;
    TestShaderMain_Streams param = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Compute_PS(_streams, param);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SMAIN = stage_input.PS_IN_SMAIN;
    PS_IN_SBASE = stage_input.PS_IN_SBASE;
    frag_main();
}
