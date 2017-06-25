#version 450

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

layout(location = 0) in vec2 VS_IN_TexCoord;
layout(location = 1) in vec4 VS_IN_Position;
layout(location = 0) out vec2 VS_OUT_TexCoord;
layout(location = 1) out vec4 VS_OUT_ShadingPosition;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = VS_IN_TexCoord;
    _streams.Position_id2 = VS_IN_Position;
    _streams.ShadingPosition_id1 = PerDraw_var.SpriteBase_MatrixTransform * _streams.Position_id2;
    VS_OUT_TexCoord = _streams.TexCoord_id0;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id1;
}

