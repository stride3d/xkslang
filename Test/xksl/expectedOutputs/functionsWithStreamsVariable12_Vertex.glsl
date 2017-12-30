#version 450

struct TestIComposition_Streams
{
    vec3 sComp;
    int _unused;
};

struct TestShaderBase_Streams
{
    vec3 sBase;
    int _unused;
};

struct TestShaderMain_Streams
{
    vec3 sMain;
    vec3 sBase;
    int _unused;
};

struct VS_STREAMS
{
    vec3 sComp_id0;
    vec3 sMain_id1;
    vec3 sBase_id2;
};

layout(location = 0) in vec3 VS_IN_sMain;
layout(location = 1) in vec3 VS_IN_sBase;
layout(location = 0) out vec3 VS_OUT_sMain;
layout(location = 1) out vec3 VS_OUT_sBase;

TestShaderMain_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestShaderMain_Streams res = TestShaderMain_Streams(_streams.sMain_id1, _streams.sBase_id2, 0);
    return res;
}

TestShaderBase_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(TestShaderMain_Streams s)
{
    TestShaderBase_Streams r = TestShaderBase_Streams(s.sBase, s._unused);
    return r;
}

void TestShaderBase_Compute(inout TestShaderBase_Streams s)
{
    s.sBase = vec3(1.0);
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(TestShaderMain_Streams s)
{
    TestIComposition_Streams r = TestIComposition_Streams(vec3(0.0), s._unused);
    return r;
}

void o0S5C0_TestIComposition_ComputeComp(inout VS_STREAMS _streams, TestShaderBase_Streams s)
{
    _streams.sComp_id0 = s.sBase;
}

void TestShaderMain_Compute(inout VS_STREAMS _streams, inout TestShaderMain_Streams s)
{
    s.sMain = vec3(2.0);
    TestShaderMain_Streams param = s;
    TestShaderBase_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param);
    TestShaderBase_Compute(param_1);
    TestShaderMain_Streams backup = TestShaderMain__getStreams(_streams);
    TestShaderMain_Streams param_2 = backup;
    TestIComposition_Streams param_3 = TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(param_2);
    o0S5C0_TestIComposition_ComputeComp(_streams, param_3);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec3(0.0), vec3(0.0), vec3(0.0));
    _streams.sMain_id1 = VS_IN_sMain;
    _streams.sBase_id2 = VS_IN_sBase;
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(_streams, param);
    VS_OUT_sMain = _streams.sMain_id1;
    VS_OUT_sBase = _streams.sBase_id2;
}

