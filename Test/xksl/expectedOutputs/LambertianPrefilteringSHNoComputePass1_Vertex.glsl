#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec2 TexCoord_id0;
    vec4 ShadingPosition_id1;
    vec4 Position_id2;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 SpriteBase_MatrixTransform;
} PerDraw_var;

in vec2 VS_IN_TEXCOORD0;
in vec4 VS_IN_POSITION;
out vec2 VS_OUT_TexCoord;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = VS_IN_TEXCOORD0;
    _streams.Position_id2 = VS_IN_POSITION;
    _streams.ShadingPosition_id1 = PerDraw_var.SpriteBase_MatrixTransform * _streams.Position_id2;
    VS_OUT_TexCoord = _streams.TexCoord_id0;
    gl_Position = _streams.ShadingPosition_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

