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

layout(location = 0) in vec4 VS_IN_s1;
layout(location = 1) in vec4 VS_IN_s2;
layout(location = 2) in int VS_IN_b1;
layout(location = 3) in float VS_IN_b2;

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0);
    return res;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), 0, 0.0);
    _streams.s1_id0 = VS_IN_s1;
    _streams.s2_id1 = VS_IN_s2;
    _streams.b1_id2 = VS_IN_b1;
    _streams.b2_id3 = VS_IN_b2;
    ShaderMain_Streams backup = ShaderMain__getStreams(_streams);
}

