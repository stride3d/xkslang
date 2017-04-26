#version 450

struct PS_STREAMS
{
    vec4 inCol_id0;
    float f1_id1;
    vec4 color_id2;
    float f2_id3;
};

layout(location = 0) in vec4 PS_IN_inCol;
layout(location = 1) in float PS_IN_f1;
layout(location = 0) out vec4 PS_OUT_color;
layout(location = 1) out float PS_OUT_f2;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), 0.0, vec4(0.0), 0.0);
    _streams.inCol_id0 = PS_IN_inCol;
    _streams.f1_id1 = PS_IN_f1;
    _streams.f2_id3 = _streams.f1_id1;
    _streams.color_id2 = _streams.inCol_id0;
    PS_OUT_color = _streams.color_id2;
    PS_OUT_f2 = _streams.f2_id3;
}

