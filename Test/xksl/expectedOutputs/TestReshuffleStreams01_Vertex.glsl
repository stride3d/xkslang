#version 450

struct VS_IN
{
    vec2 VertexPosition_id0;
    float aFloat_id1;
};

struct VS_OUT
{
    vec2 Position_id0;
};

struct VS_STREAMS
{
    vec2 Position_id0;
    vec2 VertexPosition_id1;
    float aFloat_id2;
};

VS_OUT main(VS_IN __input__)
{
    VS_STREAMS _streams;
    _streams.VertexPosition_id1 = __input__.VertexPosition_id0;
    _streams.aFloat_id2 = __input__.aFloat_id1;
    _streams.Position_id0 = vec2(0.0, 1.0 + _streams.aFloat_id2) + _streams.VertexPosition_id1;
    VS_OUT __output__;
    __output__.Position_id0 = _streams.Position_id0;
    return __output__;
}

