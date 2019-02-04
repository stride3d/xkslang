#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec2 Position_id0;
};

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0));
    _streams.Position_id0 = vec2(0.0, 1.0);
    gl_Position = _streams.Position_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

