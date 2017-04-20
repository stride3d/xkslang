#version 450

struct VS_OUT
{
    vec2 Position_id0;
};

struct PS_OUT
{
    vec4 ColorTarget_id0;
};

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

vec4 ShaderMain_Compute(PS_STREAMS _streams)
{
    return vec4(_streams.Position_id0, 0.0, 1.0);
}

PS_OUT main(VS_OUT __input__)
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0));
    _streams.Position_id0 = __input__.Position_id0;
    _streams.ColorTarget_id1 = ShaderMain_Compute(_streams);
    PS_OUT __output__ = PS_OUT(vec4(0.0));
    __output__.ColorTarget_id0 = _streams.ColorTarget_id1;
    return __output__;
}

