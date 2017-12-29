#version 450

struct TestShaderBaseA_Streams
{
    vec3 sBaseA;
    int _unused;
};

struct TestShaderBaseB_Streams
{
    vec3 sBaseB;
    vec3 sBaseA;
    int _unused;
};

struct TestShaderMain_Streams
{
    vec3 sMain;
    vec3 sBaseB;
    vec3 sBaseA;
    int _unused;
};

struct VS_STREAMS
{
    vec3 sMain_id0;
    vec3 sBaseB_id1;
    vec3 sBaseA_id2;
};

layout(location = 0) in vec3 VS_IN_sMain;
layout(location = 1) in vec3 VS_IN_sBaseB;
layout(location = 2) in vec3 VS_IN_sBaseA;
layout(location = 0) out vec3 VS_OUT_sMain;
layout(location = 1) out vec3 VS_OUT_sBaseB;
layout(location = 2) out vec3 VS_OUT_sBaseA;

TestShaderMain_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestShaderMain_Streams res = TestShaderMain_Streams(_streams.sMain_id0, _streams.sBaseB_id1, _streams.sBaseA_id2, 0);
    return res;
}

TestShaderBaseB_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseBStreams(TestShaderMain_Streams s)
{
    TestShaderBaseB_Streams r = TestShaderBaseB_Streams(s.sBaseB, s.sBaseA, s._unused);
    return r;
}

TestShaderBaseA_Streams TestShaderBaseB__ConvertTestShaderBaseBStreamsToTestShaderBaseAStreams(TestShaderBaseB_Streams s)
{
    TestShaderBaseA_Streams r = TestShaderBaseA_Streams(s.sBaseA, s._unused);
    return r;
}

void TestShaderBaseA_Compute(inout TestShaderBaseA_Streams s)
{
    s.sBaseA = vec3(1.0);
}

void TestShaderBaseB_Compute(inout TestShaderBaseB_Streams s)
{
    s.sBaseB = vec3(2.0);
    TestShaderBaseB_Streams param = s;
    TestShaderBaseA_Streams param_1 = TestShaderBaseB__ConvertTestShaderBaseBStreamsToTestShaderBaseAStreams(param);
    TestShaderBaseA_Compute(param_1);
}

void TestShaderMain_Compute(inout TestShaderMain_Streams s)
{
    s.sMain = vec3(1.0);
    TestShaderMain_Streams param = s;
    TestShaderBaseB_Streams param_1 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseBStreams(param);
    TestShaderBaseB_Compute(param_1);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec3(0.0), vec3(0.0), vec3(0.0));
    _streams.sMain_id0 = VS_IN_sMain;
    _streams.sBaseB_id1 = VS_IN_sBaseB;
    _streams.sBaseA_id2 = VS_IN_sBaseA;
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(param);
    VS_OUT_sMain = _streams.sMain_id0;
    VS_OUT_sBaseB = _streams.sBaseB_id1;
    VS_OUT_sBaseA = _streams.sBaseA_id2;
}

