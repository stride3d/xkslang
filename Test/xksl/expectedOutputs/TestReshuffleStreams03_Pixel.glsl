#version 450

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

void main()
{
    PS_STREAMS _streams;
    vec4 _12 = ShaderMain_Compute(_streams);
    _streams.ColorTarget_id1 = _12 + vec4(_streams.Position_id0, 0.0, 1.0);
}

