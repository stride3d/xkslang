#version 450

struct VS_STREAMS
{
    mat2x3 aMat23_id0;
};

layout(location = 0) in mat2x3 VS_IN_aMat23;
layout(location = 0) out mat2x3 VS_OUT_aMat23;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(mat2x3(vec3(0.0), vec3(0.0)));
    _streams.aMat23_id0 = VS_IN_aMat23;
    VS_OUT_aMat23 = _streams.aMat23_id0;
}

