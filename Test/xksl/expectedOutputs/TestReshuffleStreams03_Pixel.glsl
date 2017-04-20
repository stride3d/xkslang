#version 450

struct PS_OUT
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

vec4 ShaderMain_Compute(inout PS_STREAMS _streams)
{
    _streams.Position_id0 = vec2(1.0);
    return vec4(_streams.Position_id0, 0.0, 0.0);
}

PS_OUT main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0));
    vec4 _12 = ShaderMain_Compute(_streams);
    _streams.ColorTarget_id1 = _12 + vec4(_streams.Position_id0, 0.0, 1.0);
    PS_OUT __output__ = PS_OUT(vec2(0.0), vec4(0.0));
    __output__.Position_id0 = _streams.Position_id0;
    __output__.ColorTarget_id1 = _streams.ColorTarget_id1;
    return __output__;
}

