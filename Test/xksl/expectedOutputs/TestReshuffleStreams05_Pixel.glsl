#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 Position_id0;
    vec4 ColorTarget_id1;
};

in vec2 PS_IN_SV_POSITION;
out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0));
    _streams.Position_id0 = PS_IN_SV_POSITION;
    _streams.ColorTarget_id1 = vec4(_streams.Position_id0, 0.0, 1.0);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

