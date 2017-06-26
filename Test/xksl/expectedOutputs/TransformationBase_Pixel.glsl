#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
};

layout(location = 0) in vec4 PS_IN_ShadingPosition;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
}

