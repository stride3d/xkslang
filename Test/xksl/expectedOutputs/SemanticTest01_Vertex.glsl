#version 450

struct VS_STREAMS
{
    vec4 Position_id0;
    vec4 ShadingPosition_id1;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 ShaderMain_MatrixTransform;
} PerDraw_var;

layout(location = 0) in vec4 VS_IN_Position;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.Position_id0 = VS_IN_Position;
    _streams.ShadingPosition_id1 = PerDraw_var.ShaderMain_MatrixTransform * _streams.Position_id0;
}

