#version 450

struct VS_STREAMS
{
    vec2 Position_id0;
};

layout(location = 0) out vec2 VS_OUT_Position;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0));
    _streams.Position_id0 = vec2(0.0, 1.0);
    VS_OUT_Position = _streams.Position_id0;
}

