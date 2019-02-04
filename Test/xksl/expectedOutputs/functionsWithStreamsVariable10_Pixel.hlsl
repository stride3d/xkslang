struct TestShaderBase_Streams
{
    int sBase;
    int _unused;
};

struct TestShaderMain_Streams
{
    float3 sMain;
    int sBase;
    int _unused;
};

struct PS_STREAMS
{
    float3 sMain_id0;
    int sBase_id1;
};

static const PS_STREAMS _124 = { 0.0f.xxx, 0 };

static float3 PS_IN_SMAIN;
static int PS_IN_SBASE;

struct SPIRV_Cross_Input
{
    int PS_IN_SBASE : SBASE;
    float3 PS_IN_SMAIN : SMAIN;
};

TestShaderMain_Streams TestShaderMain__getStreams_PS(PS_STREAMS _streams)
{
    TestShaderMain_Streams _114 = { _streams.sMain_id0, _streams.sBase_id1, 0 };
    TestShaderMain_Streams res = _114;
    return res;
}

TestShaderBase_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(TestShaderMain_Streams s)
{
    TestShaderBase_Streams _61 = { s.sBase, s._unused };
    TestShaderBase_Streams r = _61;
    return r;
}

void TestShaderBase_Compute(inout TestShaderBase_Streams s)
{
    s.sBase = 1;
}

void TestShaderMain_Compute(inout TestShaderMain_Streams s)
{
    s.sMain = 2.0f.xxx;
    TestShaderMain_Streams param = s;
    TestShaderBase_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param);
    TestShaderBase_Compute(param_1);
}

void TestShaderBase_Compute(int i, inout TestShaderBase_Streams s)
{
    s.sBase = i;
}

void TestShaderMain_Compute(int i, inout TestShaderMain_Streams s)
{
    s.sMain = 3.0f.xxx;
    TestShaderMain_Streams param = s;
    int param_1 = i;
    TestShaderBase_Streams param_2 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param);
    TestShaderBase_Compute(param_1, param_2);
}

void TestShaderBase_Compute(inout TestShaderBase_Streams s1, TestShaderBase_Streams s2)
{
    s1.sBase = s2.sBase;
}

void TestShaderMain_Compute(inout TestShaderMain_Streams s1, TestShaderMain_Streams s2)
{
    s1.sMain = 4.0f.xxx;
    TestShaderMain_Streams param = s1;
    TestShaderMain_Streams param_1 = s2;
    TestShaderBase_Streams param_2 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param);
    TestShaderBase_Streams param_3 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param_1);
    TestShaderBase_Compute(param_2, param_3);
}

void frag_main()
{
    PS_STREAMS _streams = _124;
    _streams.sMain_id0 = PS_IN_SMAIN;
    _streams.sBase_id1 = PS_IN_SBASE;
    TestShaderMain_Streams backup = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Streams param = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Compute(param);
    int param_1 = 1;
    TestShaderMain_Streams param_2 = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Compute(param_1, param_2);
    TestShaderMain_Streams param_3 = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Streams param_4 = backup;
    TestShaderMain_Compute(param_3, param_4);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SMAIN = stage_input.PS_IN_SMAIN;
    PS_IN_SBASE = stage_input.PS_IN_SBASE;
    frag_main();
}
