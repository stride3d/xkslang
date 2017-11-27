#version 450

struct VS_STREAMS
{
    vec2 TexCoord_id0;
    vec2 aStream_id1;
};

layout(location = 0) in vec2 VS_IN_TexCoord;
layout(location = 0) out vec2 VS_OUT_aStream;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0), vec2(0.0));
    _streams.TexCoord_id0 = VS_IN_TexCoord;
    _streams.aStream_id1 = _streams.TexCoord_id0;
    VS_OUT_aStream = _streams.aStream_id1;
}

