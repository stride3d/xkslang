#version 450

struct VS_STREAMS
{
    vec2 Position_id0;
    vec2 VertexPosition_id1;
    float aFloat_id2;
};

layout(location = 0) in vec2 VS_IN_VertexPosition;
layout(location = 1) in float VS_IN_aFloat;
layout(location = 0) out vec2 VS_OUT_Position;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0), vec2(0.0), 0.0);
    _streams.VertexPosition_id1 = VS_IN_VertexPosition;
    _streams.aFloat_id2 = VS_IN_aFloat;
    _streams.Position_id0 = vec2(0.0, 1.0 + _streams.aFloat_id2) + _streams.VertexPosition_id1;
    VS_OUT_Position = _streams.Position_id0;
}

