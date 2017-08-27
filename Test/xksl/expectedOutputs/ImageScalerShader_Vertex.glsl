#version 450

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

layout(location = 0) in vec4 VS_IN_Position;
layout(location = 1) in vec2 VS_IN_TexCoord;
layout(location = 0) out vec4 VS_OUT_ShadingPosition;
layout(location = 1) out vec2 VS_OUT_TexCoord;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), vec2(0.0));
    _streams.Position_id1 = VS_IN_Position;
    _streams.TexCoord_id2 = VS_IN_TexCoord;
    _streams.ShadingPosition_id0 = PerDraw_var.SpriteBase_MatrixTransform * _streams.Position_id1;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_TexCoord = _streams.TexCoord_id2;
}

