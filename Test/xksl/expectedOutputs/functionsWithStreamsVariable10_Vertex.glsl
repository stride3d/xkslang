#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct TestShaderBase_Streams
{
    int sBase;
    int _unused;
};

struct TestShaderMain_Streams
{
    vec3 sMain;
    int sBase;
    int _unused;
};

struct VS_STREAMS
{
    vec3 sMain_id0;
    int sBase_id1;
};

in vec3 VS_IN_SMAIN;
in int VS_IN_SBASE;
out vec3 VS_OUT_sMain;
out int VS_OUT_sBase;

TestShaderMain_Streams TestShaderMain__getStreams(VS_STREAMS _streams)
{
    TestShaderMain_Streams res = TestShaderMain_Streams(_streams.sMain_id0, _streams.sBase_id1, 0);
    return res;
}

TestShaderBase_Streams TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(TestShaderMain_Streams s)
{
    TestShaderBase_Streams r = TestShaderBase_Streams(s.sBase, s._unused);
    return r;
}

void TestShaderBase_Compute(inout TestShaderBase_Streams s)
{
    s.sBase = 1;
}

void TestShaderMain_Compute(inout TestShaderMain_Streams s)
{
    s.sMain = vec3(2.0);
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
    s.sMain = vec3(3.0);
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
    s1.sMain = vec3(4.0);
    TestShaderMain_Streams param = s1;
    TestShaderMain_Streams param_1 = s2;
    TestShaderBase_Streams param_2 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param);
    TestShaderBase_Streams param_3 = TestShaderMain__ConvertTestShaderMainStreamsToTestShaderBaseStreams(param_1);
    TestShaderBase_Compute(param_2, param_3);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec3(0.0), 0);
    _streams.sMain_id0 = VS_IN_SMAIN;
    _streams.sBase_id1 = VS_IN_SBASE;
    TestShaderMain_Streams backup = TestShaderMain__getStreams(_streams);
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(param);
    int param_1 = 1;
    TestShaderMain_Streams param_2 = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(param_1, param_2);
    TestShaderMain_Streams param_3 = TestShaderMain__getStreams(_streams);
    TestShaderMain_Streams param_4 = backup;
    TestShaderMain_Compute(param_3, param_4);
    VS_OUT_sMain = _streams.sMain_id0;
    VS_OUT_sBase = _streams.sBase_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

