#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    float Depth_id2;
    float CustomStream_id3;
};

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 0) out vec4 PS_OUT_ColorTarget;
layout(location = 1) out float PS_OUT_Depth;
layout(location = 2) out float PS_OUT_CustomStream;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), 0.0, 0.0);
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.CustomStream_id3 = 1.0;
    _streams.Depth_id2 = 0.0;
    _streams.ColorTarget_id1 = vec4(_streams.ShadingPosition_id0.x, _streams.ShadingPosition_id0.y, 1.0, 1.0);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    PS_OUT_Depth = _streams.Depth_id2;
    PS_OUT_CustomStream = _streams.CustomStream_id3;
}

