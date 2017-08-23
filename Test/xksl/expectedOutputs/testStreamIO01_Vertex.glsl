#version 450

struct VS_STREAMS
{
    mat3 tangentToWorld_id0;
};

layout(location = 0) in mat3 VS_IN_tangentToWorld;
layout(location = 0) out mat3 VS_OUT_tangentToWorld;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(mat3(vec3(0.0), vec3(0.0), vec3(0.0)));
    _streams.tangentToWorld_id0 = VS_IN_tangentToWorld;
    VS_OUT_tangentToWorld = _streams.tangentToWorld_id0;
}

