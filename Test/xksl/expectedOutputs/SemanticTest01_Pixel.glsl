#version 450

struct PS_STREAMS
{
    vec4 ColorTarget_id0;
    float Depth_id1;
};

layout(location = 0) out vec4 PS_OUT_ColorTarget;
layout(location = 1) out float PS_OUT_Depth;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), 0.0);
    _streams.Depth_id1 = 0.0;
    _streams.ColorTarget_id0 = vec4(1.0);
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
    PS_OUT_Depth = _streams.Depth_id1;
}

