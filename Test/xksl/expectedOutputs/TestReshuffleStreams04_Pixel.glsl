#version 450

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

layout(location = 0) in vec2 PS_IN_Position;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec4 ShaderMain_Compute(PS_STREAMS _streams, float p)
{
    return vec4(_streams.Position_id0, p, 1.0);
}

vec4 ShaderMain_Compute(PS_STREAMS _streams)
{
    float param = 5.0;
    return ShaderMain_Compute(_streams, param);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0));
    _streams.Position_id0 = PS_IN_Position;
    _streams.ColorTarget_id1 = ShaderMain_Compute(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

