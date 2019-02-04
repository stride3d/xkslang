#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

in vec2 PS_IN_POSITION;
out vec4 PS_OUT_ColorTarget;

vec4 ShaderMain_Compute(PS_STREAMS _streams)
{
    return vec4(_streams.Position_id0, 0.0, 1.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0));
    _streams.Position_id0 = PS_IN_POSITION;
    _streams.ColorTarget_id1 = ShaderMain_Compute(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

