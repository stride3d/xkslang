#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 Position_id1;
    vec2 TexCoord_id2;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 SpriteBase_MatrixTransform;
} PerDraw_var;

in vec4 VS_IN_POSITION;
in vec2 VS_IN_TEXCOORD0;
out vec2 VS_OUT_TexCoord;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), vec2(0.0));
    _streams.Position_id1 = VS_IN_POSITION;
    _streams.TexCoord_id2 = VS_IN_TEXCOORD0;
    _streams.ShadingPosition_id0 = PerDraw_var.SpriteBase_MatrixTransform * _streams.Position_id1;
    gl_Position = _streams.ShadingPosition_id0;
    VS_OUT_TexCoord = _streams.TexCoord_id2;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

