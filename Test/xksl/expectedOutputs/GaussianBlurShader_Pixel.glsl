#version 450

struct PS_STREAMS
{
    vec2 TexCoord_id0;
    vec4 ShadingPosition_id1;
    vec4 ColorTarget_id2;
};

layout(std140) uniform Globals
{
    vec2 GaussianBlurShader_OffsetsWeights[3];
    vec2 Texturing_Texture0TexelSize;
} Globals_var;

uniform sampler2D SPIRV_Cross_CombinedTexturing_Texture0Texturing_LinearSampler;

layout(location = 0) in vec2 PS_IN_TexCoord;
layout(location = 1) in vec4 PS_IN_ShadingPosition;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec4 GaussianBlurShader_3_false__Shading(PS_STREAMS _streams)
{
    vec2 direction = vec2(1.0, 0.0) * Globals_var.Texturing_Texture0TexelSize;
    vec3 value = texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_LinearSampler, _streams.TexCoord_id0).xyz * Globals_var.GaussianBlurShader_OffsetsWeights[0].y;
    for (int i = 1; i < 3; i++)
    {
        value += (texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_LinearSampler, _streams.TexCoord_id0 - (direction * Globals_var.GaussianBlurShader_OffsetsWeights[i].x)).xyz * Globals_var.GaussianBlurShader_OffsetsWeights[i].y);
        value += (texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_LinearSampler, _streams.TexCoord_id0 + (direction * Globals_var.GaussianBlurShader_OffsetsWeights[i].x)).xyz * Globals_var.GaussianBlurShader_OffsetsWeights[i].y);
    }
    return vec4(value, 1.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id1 = PS_IN_ShadingPosition;
    _streams.ColorTarget_id2 = GaussianBlurShader_3_false__Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

