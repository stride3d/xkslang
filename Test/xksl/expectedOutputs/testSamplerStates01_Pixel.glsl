#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 aStream_id0;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_SamplerDefault;

in vec2 PS_IN_ASTREAM;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0));
    _streams.aStream_id0 = PS_IN_ASTREAM;
    vec2 uv2 = _streams.aStream_id0;
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_SamplerDefault, uv2);
}

