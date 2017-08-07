#version 450

struct VS_STREAMS
{
    vec4 outStreamA_id0;
};

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.outStreamA_id0 = vec4(5.0);
}

