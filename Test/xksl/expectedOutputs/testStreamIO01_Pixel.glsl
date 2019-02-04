#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    mat3 tangentToWorld_id0;
};

in mat3 PS_IN_TANGENTTOWORLD;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(mat3(vec3(0.0), vec3(0.0), vec3(0.0)));
    _streams.tangentToWorld_id0 = PS_IN_TANGENTTOWORLD;
    mat3 v2 = _streams.tangentToWorld_id0;
}

