#version 450

struct ShaderMain_Streams
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

layout(location = 0) in int VS_IN_b1;
layout(location = 1) in float VS_IN_b2;
layout(location = 0) out vec4 VS_OUT_s1;
layout(location = 1) out vec4 VS_OUT_s2;

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0);
    return res;
}

void ShaderMain__setStreams(inout VS_STREAMS _streams, ShaderMain_Streams _s)
{
    _streams.s1_id0 = _s.s1;
    _streams.s2_id1 = _s.s2;
    _streams.b1_id2 = _s.b1;
    _streams.b2_id3 = _s.b2;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), 0, 0.0);
    _streams.b1_id2 = VS_IN_b1;
    _streams.b2_id3 = VS_IN_b2;
    _streams.s1_id0 = vec4(0.0, 1.0, 2.0, 3.0);
    _streams.s2_id1 = vec4(4.0, 5.0, 6.0, 7.0);
    ShaderMain_Streams backup = ShaderMain__getStreams(_streams);
    _streams.s2_id1 = vec4(8.0, 9.0, 10.0, 11.0);
    ShaderMain_Streams param = backup;
    ShaderMain__setStreams(_streams, param);
    VS_OUT_s1 = _streams.s1_id0;
    VS_OUT_s2 = _streams.s2_id1;
}

