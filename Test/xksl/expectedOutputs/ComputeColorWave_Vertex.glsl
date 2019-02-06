#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec2 TexCoord_id0;
};

in vec2 VS_IN_TEXCOORD0;
out vec2 VS_OUT_TexCoord;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0));
    _streams.TexCoord_id0 = VS_IN_TEXCOORD0;
    VS_OUT_TexCoord = _streams.TexCoord_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

