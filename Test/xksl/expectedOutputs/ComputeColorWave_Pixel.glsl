#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 TexCoord_id0;
};

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

in vec2 PS_IN_TEXCOORD0;

vec4 ComputeColorWave_1_2_3_645541__Compute(PS_STREAMS _streams)
{
    float phase = length(_streams.TexCoord_id0 - vec2(0.5));
    return vec4(sin((((phase + (PerFrame_var.Global_Time * 3.6455414295196533203125)) * 2.0) * 3.1400001049041748046875) * 1.0) * 2.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0));
    _streams.TexCoord_id0 = PS_IN_TEXCOORD0;
}

