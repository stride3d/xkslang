#version 450

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

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 0) out vec4 PS_OUT_ColorTarget;
layout(location = 1) out float PS_OUT_Depth;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), 0.0);
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.Depth_id2 = _streams.ShadingPosition_id0.w + Globals_var.ShaderMain_depthOffset;
    _streams.ColorTarget_id1 = vec4(_streams.ShadingPosition_id0.x, _streams.ShadingPosition_id0.y, 1.0, Globals_var.ShaderMain_depthOffset);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    PS_OUT_Depth = _streams.Depth_id2;
}

