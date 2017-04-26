#version 450

struct VS_STREAMS
{
    vec4 inCol_id0;
    float f1_id1;
};

layout(location = 0) in vec4 VS_IN_inCol;
layout(location = 1) in float VS_IN_f1;
layout(location = 0) out vec4 VS_OUT_inCol;
layout(location = 1) out float VS_OUT_f1;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), 0.0);
    _streams.inCol_id0 = VS_IN_inCol;
    _streams.f1_id1 = VS_IN_f1;
    VS_OUT_inCol = _streams.inCol_id0;
    VS_OUT_f1 = _streams.f1_id1;
}

