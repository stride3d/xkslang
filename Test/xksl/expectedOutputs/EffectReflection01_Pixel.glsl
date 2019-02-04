#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    float Depth_id2;
};

layout(std140) uniform Globals
{
    float ShaderMain_depthOffset;
} Globals_var;

in vec4 PS_IN_SV_Position;
out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), 0.0);
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.Depth_id2 = _streams.ShadingPosition_id0.w + Globals_var.ShaderMain_depthOffset;
    _streams.ColorTarget_id1 = vec4(_streams.ShadingPosition_id0.x, _streams.ShadingPosition_id0.y, 1.0, Globals_var.ShaderMain_depthOffset);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    gl_FragDepth = _streams.Depth_id2;
}

