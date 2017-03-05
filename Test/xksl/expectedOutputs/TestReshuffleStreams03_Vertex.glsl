#version 450

struct VS_STREAMS
{
    vec2 Position_id0;
};

void main()
{
    VS_STREAMS _streams;
    _streams.Position_id0 = vec2(0.0, 1.0);
}

