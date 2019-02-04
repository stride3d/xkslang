#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    mat3 tangentToWorld_id0;
};

in mat3 VS_IN_TANGENTTOWORLD;
out mat3 VS_OUT_tangentToWorld;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(mat3(vec3(0.0), vec3(0.0), vec3(0.0)));
    _streams.tangentToWorld_id0 = VS_IN_TANGENTTOWORLD;
    VS_OUT_tangentToWorld = _streams.tangentToWorld_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

