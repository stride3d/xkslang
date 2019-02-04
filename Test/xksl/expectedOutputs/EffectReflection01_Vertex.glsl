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
    layout(row_major) mat4x3 ShaderMain_Matrix4_3;
    layout(row_major) mat2x3 ShaderMain_Matrix2_3;
    vec3 ShaderMain_ColorMatrix;
    vec4 ShaderMain_ColorArray[2];
} PerDraw_var;

layout(std140) uniform Globals
{
    float ShaderMain_depthOffset;
} Globals_var;

in vec4 VS_IN_POSITION;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.Position_id0 = VS_IN_POSITION;
    _streams.ShadingPosition_id1 = (PerDraw_var.ShaderMain_MatrixTransform * _streams.Position_id0) + vec4(PerDraw_var.ShaderMain_ColorArray[1].xyz, Globals_var.ShaderMain_depthOffset);
    gl_Position = _streams.ShadingPosition_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

