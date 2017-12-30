#version 450

struct TestIComposition_Streams
{
    vec3 sComp;
    int _unused;
};

struct TestShaderMain_Streams
{
    vec3 sMain;
    int _unused;
};

struct VS_STREAMS
{
    vec3 sComp_id0;
    vec3 sMain_id1;
};

layout(location = 0) in vec3 VS_IN_sMain;
layout(location = 0) out vec3 VS_OUT_sComp;

TestShaderMain_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestShaderMain_Streams res = TestShaderMain_Streams(_streams.sMain_id1, 0);
    return res;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(TestShaderMain_Streams s)
{
    TestIComposition_Streams r = TestIComposition_Streams(vec3(0.0), s._unused);
    return r;
}

void o0S5C0_TestIComposition_ComputeComp(inout VS_STREAMS _streams, TestShaderMain_Streams s)
{
    _streams.sComp_id0 = s.sMain;
}

void TestShaderMain_Compute(inout VS_STREAMS _streams, inout TestShaderMain_Streams s)
{
    s.sMain = vec3(2.0);
    TestShaderMain_Streams backup = TestShaderMain__getStreams(_streams);
    TestShaderMain_Streams param = backup;
    TestIComposition_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(param);
    o0S5C0_TestIComposition_ComputeComp(_streams, param_1);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec3(0.0), vec3(0.0));
    _streams.sMain_id1 = VS_IN_sMain;
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(_streams, param);
    VS_OUT_sComp = _streams.sComp_id0;
}

