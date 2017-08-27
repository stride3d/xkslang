#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    vec2 TexCoord_id2;
};

layout(std140) uniform Globals
{
    vec4 ImageScalerShader_Color;
    float ImageScalerShader_IsOnlyChannelRed;
} Globals_var;

uniform sampler2D SPIRV_Cross_CombinedTexturing_Texture0Texturing_Sampler;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec2 PS_IN_TexCoord;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec4 SpriteBase_Shading(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_Sampler, _streams.TexCoord_id2);
}

vec4 ImageScalerShader_Shading(PS_STREAMS _streams)
{
    vec4 color = SpriteBase_Shading(_streams);
    if (Globals_var.ImageScalerShader_IsOnlyChannelRed != 0.0)
    {
        color = vec4(color.xxx, 1.0);
    }
    return color * Globals_var.ImageScalerShader_Color;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), vec2(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.TexCoord_id2 = PS_IN_TexCoord;
    _streams.ColorTarget_id1 = ImageScalerShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

