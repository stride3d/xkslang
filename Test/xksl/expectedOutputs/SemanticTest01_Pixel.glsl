#version 450

struct PS_STREAMS
{
    vec4 color_id0;
    float depth_id1;
    vec4 colorB_id2;
};

layout(location = 0) out vec4 PS_OUT_color;
layout(location = 1) out float PS_OUT_depth;
layout(location = 2) out vec4 PS_OUT_colorB;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), 0.0, vec4(0.0));
    _streams.depth_id1 = 0.0;
    _streams.color_id0 = vec4(0.0);
    _streams.colorB_id2 = vec4(0.0);
    PS_OUT_color = _streams.color_id0;
    PS_OUT_depth = _streams.depth_id1;
    PS_OUT_colorB = _streams.colorB_id2;
}

