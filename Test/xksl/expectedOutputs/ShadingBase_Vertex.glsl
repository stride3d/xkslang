#version 450

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
};

layout(location = 0) in vec4 VS_IN_ShadingPosition;
layout(location = 0) out vec4 VS_OUT_ShadingPosition;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.ShadingPosition_id0 = VS_IN_ShadingPosition;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
}

