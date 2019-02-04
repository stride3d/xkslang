#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 ColorTarget_id0;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat2x3 ShaderMain_aMat23[2];
    vec4 ShaderMain_Color[4];
    vec4 ShaderMain_ColorArray[2];
} PerDraw_var;

out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0));
    _streams.ColorTarget_id0 = PerDraw_var.ShaderMain_Color[1];
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

