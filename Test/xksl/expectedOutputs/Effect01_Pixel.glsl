#version 450

struct PS_STREAMS
{
    vec2 TexCoord_id0;
    vec4 ShadingPosition_id1;
    vec4 ColorTarget_id2;
};

layout(std140) uniform Globals
{
    vec2 Effect_Center;
    float Effect_Frequency;
    float Effect_Phase;
    float Effect_Spread;
    float Effect_Amplitude;
    float Effect_InvAspectRatio;
} Globals_var;

uniform sampler2D SPIRV_Cross_CombinedTexturing_Texture0Texturing_Sampler;

layout(location = 0) in vec2 PS_IN_TexCoord;
layout(location = 1) in vec4 PS_IN_ShadingPosition;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec4 Effect_Shading(PS_STREAMS _streams)
{
    vec2 toPixel = (_streams.TexCoord_id0 - Globals_var.Effect_Center) * vec2(1.0, Globals_var.Effect_InvAspectRatio);
    float distance = length(toPixel);
    vec2 direction = normalize(toPixel);
    vec2 wave;
    wave.x = sin((Globals_var.Effect_Frequency * distance) + Globals_var.Effect_Phase);
    wave.y = cos((Globals_var.Effect_Frequency * distance) + Globals_var.Effect_Phase);
    float falloff = clamp(1.0 - distance, 0.0, 1.0);
    falloff = pow(falloff, 1.0 / Globals_var.Effect_Spread);
    vec2 uv2 = _streams.TexCoord_id0 + (direction * ((wave.x * falloff) * Globals_var.Effect_Amplitude));
    float lighting = mix(1.0, 1.0 + ((wave.x * falloff) * 0.20000000298023223876953125), clamp(Globals_var.Effect_Amplitude / 0.014999999664723873138427734375, 0.0, 1.0));
    vec4 color = texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_Sampler, uv2);
    vec3 _82 = color.xyz * lighting;
    color = vec4(_82.x, _82.y, _82.z, color.w);
    return color;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id1 = PS_IN_ShadingPosition;
    _streams.ColorTarget_id2 = Effect_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

