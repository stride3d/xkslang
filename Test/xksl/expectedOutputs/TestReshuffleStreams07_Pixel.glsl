#version 450

struct PS_STREAMS
{
    mat2x3 aMat23_id0;
};

layout(location = 0) in mat2x3 PS_IN_aMat23;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(mat2x3(vec3(0.0), vec3(0.0)));
    _streams.aMat23_id0 = PS_IN_aMat23;
    float f = _streams.aMat23_id0[0].x;
}

