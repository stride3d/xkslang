#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct TestIComposition_Streams
{
    vec3 sComp;
    int _unused;
};

struct VS_STREAMS
{
    vec3 sComp_id0;
    vec3 sMain_id1;
};

in vec3 VS_IN_SMAIN;
out vec3 VS_OUT_sComp;

TestIComposition_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestIComposition_Streams res = TestIComposition_Streams(_streams.sMain_id1, 0);
    return res;
}

TestIComposition_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(TestIComposition_Streams s)
{
    TestIComposition_Streams r = TestIComposition_Streams(vec3(0.0), s._unused);
    return r;
}

void o0S5C0_TestIComposition_ComputeComp(inout VS_STREAMS _streams, TestIComposition_Streams s)
{
    _streams.sComp_id0 = s.sComp;
}

void TestShaderMain_Compute(inout VS_STREAMS _streams, inout TestIComposition_Streams s)
{
    s.sComp = vec3(2.0);
    TestIComposition_Streams backup = TestShaderMain__getStreams(_streams);
    TestIComposition_Streams param = backup;
    TestIComposition_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestICompositionStreams(param);
    o0S5C0_TestIComposition_ComputeComp(_streams, param_1);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec3(0.0), vec3(0.0));
    _streams.sMain_id1 = VS_IN_SMAIN;
    TestIComposition_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(_streams, param);
    VS_OUT_sComp = _streams.sComp_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

