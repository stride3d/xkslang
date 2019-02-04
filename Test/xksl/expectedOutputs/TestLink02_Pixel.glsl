#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 s_int_id0;
    vec4 s_out_id1;
};

in vec4 PS_IN_S_INT;
out vec4 PS_OUT_s_out;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.s_int_id0 = PS_IN_S_INT;
    _streams.s_out_id1 = _streams.s_int_id0;
    PS_OUT_s_out = _streams.s_out_id1;
}

