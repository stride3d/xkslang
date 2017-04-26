#version 450

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

layout(location = 0) out vec2 PS_OUT_Position;
layout(location = 1) out vec4 PS_OUT_ColorTarget;

vec4 ShaderMain_Compute(inout PS_STREAMS _streams)
{
    _streams.Position_id0 = vec2(1.0);
    return vec4(_streams.Position_id0, 0.0, 0.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0));
    vec4 _12 = ShaderMain_Compute(_streams);
    _streams.ColorTarget_id1 = _12 + vec4(_streams.Position_id0, 0.0, 1.0);
    PS_OUT_Position = _streams.Position_id0;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

