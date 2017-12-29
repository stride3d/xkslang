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

struct VS_STREAMS
{
    float3 sMain_id0;
    float3 sBaseB_id1;
    float3 sBaseA_id2;
};

static float3 VS_IN_sMain;
static float3 VS_IN_sBaseB;
static float3 VS_IN_sBaseA;
static float3 VS_OUT_sMain;
static float3 VS_OUT_sBaseB;
static float3 VS_OUT_sBaseA;

struct SPIRV_Cross_Input
{
    float3 VS_IN_sMain : SMAIN;
    float3 VS_IN_sBaseB : SBASEB;
    float3 VS_IN_sBaseA : SBASEA;
};

struct SPIRV_Cross_Output
{
    float3 VS_OUT_sMain : SMAIN;
    float3 VS_OUT_sBaseB : SBASEB;
    float3 VS_OUT_sBaseA : SBASEA;
};

TestShaderMain_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestShaderMain_Streams res = { _streams.sMain_id0, _streams.sBaseB_id1, _streams.sBaseA_id2, 0 };
    return res;
}

TestShaderBaseB_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseBStreams(TestShaderMain_Streams s)
{
    TestShaderBaseB_Streams r = { s.sBaseB, s.sBaseA, s._unused };
    return r;
}

TestShaderBaseA_Streams TestShaderBaseB__ConvertTestShaderBaseBStreamsToTestShaderBaseAStreams(TestShaderBaseB_Streams s)
{
    TestShaderBaseA_Streams r = { s.sBaseA, s._unused };
    return r;
}

void TestShaderBaseA_Compute(inout TestShaderBaseA_Streams s)
{
    s.sBaseA = float3(1.0f, 1.0f, 1.0f);
}

void TestShaderBaseB_Compute(inout TestShaderBaseB_Streams s)
{
    s.sBaseB = float3(2.0f, 2.0f, 2.0f);
    TestShaderBaseB_Streams param = s;
    TestShaderBaseA_Streams param_1 = TestShaderBaseB__ConvertTestShaderBaseBStreamsToTestShaderBaseAStreams(param);
    TestShaderBaseA_Compute(param_1);
}

void TestShaderMain_Compute(inout TestShaderMain_Streams s)
{
    s.sMain = float3(1.0f, 1.0f, 1.0f);
    TestShaderMain_Streams param = s;
    TestShaderBaseB_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseBStreams(param);
    TestShaderBaseB_Compute(param_1);
}

void vert_main()
{
    VS_STREAMS _streams = { float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f) };
    _streams.sMain_id0 = VS_IN_sMain;
    _streams.sBaseB_id1 = VS_IN_sBaseB;
    _streams.sBaseA_id2 = VS_IN_sBaseA;
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(param);
    VS_OUT_sMain = _streams.sMain_id0;
    VS_OUT_sBaseB = _streams.sBaseB_id1;
    VS_OUT_sBaseA = _streams.sBaseA_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_sMain = stage_input.VS_IN_sMain;
    VS_IN_sBaseB = stage_input.VS_IN_sBaseB;
    VS_IN_sBaseA = stage_input.VS_IN_sBaseA;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_sMain = VS_OUT_sMain;
    stage_output.VS_OUT_sBaseB = VS_OUT_sBaseB;
    stage_output.VS_OUT_sBaseA = VS_OUT_sBaseA;
    return stage_output;
}
