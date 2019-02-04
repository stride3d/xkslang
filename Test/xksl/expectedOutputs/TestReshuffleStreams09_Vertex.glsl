#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    float s1_id0;
    float s3_id1;
    float s4_id2;
};

in float VS_IN_S4;
out float VS_OUT_s4;

void ShaderMain_AnotherFunctionNotDoingAnything()
{
}

float ShaderMain_FunctionUsingStreams(VS_STREAMS _streams)
{
    ShaderMain_AnotherFunctionNotDoingAnything();
    return _streams.s4_id2;
}

void ShaderMain_FunctionNotUsingStreams(VS_STREAMS _streams, int i)
{
    float f = ShaderMain_FunctionUsingStreams(_streams);
}

void ShaderMain_CommonFunctionUsingStreams(inout VS_STREAMS _streams, float f, int i)
{
    _streams.s3_id1 = f;
    int param = i;
    ShaderMain_FunctionNotUsingStreams(_streams, param);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, 0.0, 0.0);
    _streams.s4_id2 = VS_IN_S4;
    _streams.s1_id0 = 0.0;
    float param = 1.0;
    int param_1 = 0;
    ShaderMain_CommonFunctionUsingStreams(_streams, param, param_1);
    VS_OUT_s4 = _streams.s4_id2;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

