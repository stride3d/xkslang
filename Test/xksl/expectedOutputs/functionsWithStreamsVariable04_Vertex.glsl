#version 450

struct ShaderMain__streamsStruct
{
    vec4 s1;
    vec4 b1;
    int _unused;
};

struct VS_STREAMS
{
    vec4 s1_id0;
    vec4 b1_id1;
};

layout(location = 0) in vec4 VS_IN_s1;
layout(location = 1) in vec4 VS_IN_b1;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.s1_id0 = VS_IN_s1;
    _streams.b1_id1 = VS_IN_b1;
    ShaderMain__streamsStruct s1 = ShaderMain__streamsStruct(_streams.s1_id0, _streams.b1_id1, 0);
    ShaderMain__streamsStruct _tmpStreamsVar_2 = ShaderMain__streamsStruct(_streams.s1_id0, _streams.b1_id1, 0);
    ShaderMain__streamsStruct s2 = _tmpStreamsVar_2;
}

