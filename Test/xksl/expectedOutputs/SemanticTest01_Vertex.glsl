#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 Position_id0;
    vec4 ShadingPosition_id1;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 ShaderMain_MatrixTransform;
} PerDraw_var;

in vec4 VS_IN_POSITION;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.Position_id0 = VS_IN_POSITION;
    _streams.ShadingPosition_id1 = PerDraw_var.ShaderMain_MatrixTransform * _streams.Position_id0;
    gl_Position = _streams.ShadingPosition_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

