#version 450

struct PS_STREAMS
{
    mat3 tangentToWorld_id0;
};

layout(location = 0) in mat3 PS_IN_tangentToWorld;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(mat3(vec3(0.0), vec3(0.0), vec3(0.0)));
    _streams.tangentToWorld_id0 = PS_IN_tangentToWorld;
    mat3 v2 = _streams.tangentToWorld_id0;
}

