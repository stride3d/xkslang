#version 450

struct PS_STREAMS
{
    vec4 s_int_id0;
    vec4 s_out_id1;
};

layout(location = 0) in vec4 PS_IN_s_int;
layout(location = 0) out vec4 PS_OUT_s_out;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.s_int_id0 = PS_IN_s_int;
    _streams.s_out_id1 = _streams.s_int_id0;
    PS_OUT_s_out = _streams.s_out_id1;
}

