#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 outStreamA_id0;
};

out vec4 PS_OUT_outStreamA;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0));
    _streams.outStreamA_id0 = vec4(2.0);
    PS_OUT_outStreamA = _streams.outStreamA_id0;
}

