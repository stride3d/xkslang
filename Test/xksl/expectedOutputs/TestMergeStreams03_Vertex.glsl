#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 outStreamA_id0;
};

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.outStreamA_id0 = vec4(5.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

