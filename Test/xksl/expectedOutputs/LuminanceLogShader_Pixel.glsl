#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 TexCoord_id0;
    vec4 ShadingPosition_id1;
    vec4 ColorTarget_id2;
};

uniform sampler2D SPIRV_Cross_CombinedTexturing_Texture0Texturing_PointSampler;

in vec2 PS_IN_TEXCOORD0;
in vec4 PS_IN_SV_Position;
out vec4 PS_OUT_ColorTarget;

float LuminanceUtils_Luma(vec3 color)
{
    return max(dot(color, vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625)), 9.9999997473787516355514526367188e-05);
}

float LuminanceLogShader_GetLuminance(vec3 color)
{
    vec3 param = color;
    return LuminanceUtils_Luma(param);
}

vec4 LuminanceLogShader_Shading(PS_STREAMS _streams)
{
    vec3 color = texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_PointSampler, _streams.TexCoord_id0).xyz;
    vec3 param = color;
    float lum = max(0.001000000047497451305389404296875, LuminanceLogShader_GetLuminance(param));
    return vec4(log2(lum), 1.0, 1.0, 1.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = PS_IN_TEXCOORD0;
    _streams.ShadingPosition_id1 = PS_IN_SV_Position;
    _streams.ColorTarget_id2 = LuminanceLogShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

