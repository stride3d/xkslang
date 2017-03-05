#version 450

struct VS_OUT
{
    vec2 Position_id0;
};

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

vec4 ShaderMain_Compute(PS_STREAMS _streams, float p)
{
    return vec4(_streams.Position_id0, p, 1.0);
}

vec4 ShaderMain_Compute(PS_STREAMS _streams)
{
    float param = 5.0;
    return ShaderMain_Compute(_streams, param);
}

void main(VS_OUT __input__)
{
    PS_STREAMS _streams;
    _streams.Position_id0 = __input__.Position_id0;
    _streams.ColorTarget_id1 = ShaderMain_Compute(_streams);
}

