#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

out vec2 PS_OUT_Position;
out vec4 PS_OUT_ColorTarget;

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

