#version 450

struct VS_STREAMS
{
    vec4 Position_id0;
    vec4 ShadingPosition_id1;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 ShaderMain_MatrixTransform;
    layout(row_major) mat4x3 ShaderMain_Matrix4_3;
    vec3 ShaderMain_ColorMatrix;
    vec4 ShaderMain_ColorArray[2];
} PerDraw_var;

layout(std140) uniform globalCbuffer
{
    float ShaderMain_depthOffset;
} globalCbuffer_var;

layout(location = 0) in vec4 VS_IN_Position;
layout(location = 0) out vec4 VS_OUT_ShadingPosition;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.Position_id0 = VS_IN_Position;
    _streams.ShadingPosition_id1 = (PerDraw_var.ShaderMain_MatrixTransform * _streams.Position_id0) + vec4(PerDraw_var.ShaderMain_ColorArray[1].xyz, globalCbuffer_var.ShaderMain_depthOffset);
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id1;
}

