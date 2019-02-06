#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
};

in vec4 PS_IN_SV_Position;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
}

