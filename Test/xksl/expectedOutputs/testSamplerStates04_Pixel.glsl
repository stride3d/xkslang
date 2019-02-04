#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 aStream_id0;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler01;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler02;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler03;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler04;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler05;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler06;

in vec2 PS_IN_ASTREAM;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0));
    _streams.aStream_id0 = PS_IN_ASTREAM;
    vec2 uv2 = _streams.aStream_id0;
    vec4 f01 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler01, uv2);
    vec4 f02 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler02, uv2);
    vec4 f03 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler03, uv2);
    vec4 f04 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler04, uv2);
    vec4 f05 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler05, uv2);
    vec4 f06 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler06, uv2);
}

