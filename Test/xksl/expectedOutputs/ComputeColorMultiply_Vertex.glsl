#version 450

struct VS_STREAMS
{
    vec2 TexCoord_id0;
};

layout(location = 0) in vec2 VS_IN_TexCoord;
layout(location = 0) out vec2 VS_OUT_TexCoord;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0));
    _streams.TexCoord_id0 = VS_IN_TexCoord;
    VS_OUT_TexCoord = _streams.TexCoord_id0;
}

