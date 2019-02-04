#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

in vec3 VS_IN_SMAIN;
in vec3 VS_IN_SBASEB;
in vec3 VS_IN_SBASEA;
out vec3 VS_OUT_sMain;
out vec3 VS_OUT_sBaseB;
out vec3 VS_OUT_sBaseA;

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
    _streams.sMain_id0 = VS_IN_SMAIN;
    _streams.sBaseB_id1 = VS_IN_SBASEB;
    _streams.sBaseA_id2 = VS_IN_SBASEA;
    TestShaderMain_Streams param = TestShaderMain__getStreams(_streams);
    TestShaderMain_Compute(param);
    VS_OUT_sMain = _streams.sMain_id0;
    VS_OUT_sBaseB = _streams.sBaseB_id1;
    VS_OUT_sBaseA = _streams.sBaseA_id2;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

