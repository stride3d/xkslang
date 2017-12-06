#version 450

struct ShaderMain__streamsStruct
{
    vec4 s1;
    vec4 s2;
    int b1;
    float b2;
    int _unused;
};

struct VS_STREAMS
{
    vec4 s1_id0;
    vec4 s2_id1;
    int b1_id2;
    float b2_id3;
};

layout(location = 0) in vec4 VS_IN_s1;
layout(location = 1) in vec4 VS_IN_s2;
layout(location = 2) in int VS_IN_b1;
layout(location = 3) in float VS_IN_b2;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), 0, 0.0);
    _streams.s1_id0 = VS_IN_s1;
    _streams.s2_id1 = VS_IN_s2;
    _streams.b1_id2 = VS_IN_b1;
    _streams.b2_id3 = VS_IN_b2;
    ShaderMain__streamsStruct s1 = ShaderMain__streamsStruct(_streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0);
    ShaderMain__streamsStruct s3 = s1;
    ShaderMain__streamsStruct s6 = ShaderMain__streamsStruct(_streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0);
    ShaderMain__streamsStruct s2 = s3;
    ShaderMain__streamsStruct _tmpStreamsVar_3 = ShaderMain__streamsStruct(_streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0);
    ShaderMain__streamsStruct s4 = _tmpStreamsVar_3;
    ShaderMain__streamsStruct s5 = s2;
    ShaderMain__streamsStruct _tmpStreamsVar_5 = ShaderMain__streamsStruct(_streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0);
    s2 = _tmpStreamsVar_5;
    _streams.s1_id0 = s1.s1;
    _streams.s2_id1 = s1.s2;
    _streams.b1_id2 = s1.b1;
    _streams.b2_id3 = s1.b2;
    ShaderMain__streamsStruct backup1 = ShaderMain__streamsStruct(_streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0);
    ShaderMain__streamsStruct backup2;
    backup2.s1 = backup1.s1;
    backup2.s2 = backup1.s2;
    backup2.b1 = backup1.b1;
    backup2.b2 = backup1.b2;
    backup2._unused = backup1._unused;
    ShaderMain__streamsStruct backup3 = backup2;
}

