#version 450

struct PS_STREAMS
{
    vec2 aStream_id0;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_SamplerDefault;

layout(location = 0) in vec2 PS_IN_aStream;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0));
    _streams.aStream_id0 = PS_IN_aStream;
    vec2 uv2 = _streams.aStream_id0;
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_SamplerDefault, uv2);
}

