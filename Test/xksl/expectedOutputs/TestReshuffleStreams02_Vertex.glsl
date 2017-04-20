#version 450

struct VS_OUT
{
    vec2 Position_id0;
};

struct VS_STREAMS
{
    vec2 Position_id0;
};

VS_OUT main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0));
    _streams.Position_id0 = vec2(0.0, 1.0);
    VS_OUT __output__ = VS_OUT(vec2(0.0));
    __output__.Position_id0 = _streams.Position_id0;
    return __output__;
}

