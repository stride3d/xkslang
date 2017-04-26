#version 450

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

layout(location = 0) in vec2 PS_IN_Position;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0));
    _streams.Position_id0 = PS_IN_Position;
    _streams.ColorTarget_id1 = vec4(_streams.Position_id0, 0.0, 1.0);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

