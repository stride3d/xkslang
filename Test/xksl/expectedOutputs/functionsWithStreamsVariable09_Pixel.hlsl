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

static float3 PS_IN_sMain;
static float3 PS_IN_sBaseB;
static float3 PS_IN_sBaseA;

struct SPIRV_Cross_Input
{
    float3 PS_IN_sMain : SMAIN;
    float3 PS_IN_sBaseB : SBASEB;
    float3 PS_IN_sBaseA : SBASEA;
};

TestShaderMain_Streams TestShaderMain__getStreams_PS(PS_STREAMS _streams)
{
    TestShaderMain_Streams res = TestShaderMain_Streams{ _streams.sMain_id0, _streams.sBaseB_id1, _streams.sBaseA_id2, 0 };
    return res;
}

TestShaderBaseB_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseBStreams(TestShaderMain_Streams s)
{
    TestShaderBaseB_Streams r = TestShaderBaseB_Streams{ s.sBaseB, s.sBaseA, s._unused };
    return r;
}

TestShaderBaseA_Streams TestShaderBaseB__ConvertTestShaderBaseBStreamsToTestShaderBaseAStreams(TestShaderBaseB_Streams s)
{
    TestShaderBaseA_Streams r = TestShaderBaseA_Streams{ s.sBaseA, s._unused };
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
    PS_STREAMS _streams = { 0.0f.xxx, 0.0f.xxx, 0.0f.xxx };
    _streams.sMain_id0 = PS_IN_sMain;
    _streams.sBaseB_id1 = PS_IN_sBaseB;
    _streams.sBaseA_id2 = PS_IN_sBaseA;
    TestShaderMain_Streams param = TestShaderMain__getStreams_PS(_streams);
    TestShaderMain_Compute(param);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_sMain = stage_input.PS_IN_sMain;
    PS_IN_sBaseB = stage_input.PS_IN_sBaseB;
    PS_IN_sBaseA = stage_input.PS_IN_sBaseA;
    frag_main();
}
