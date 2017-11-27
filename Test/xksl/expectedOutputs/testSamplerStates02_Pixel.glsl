#version 450

struct PS_STREAMS
{
    vec2 aStream_id0;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_PointSampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearSampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearBorderSampler;
uniform sampler2DShadow SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearClampCompareLessEqualSampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_AnisotropicSampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_AnisotropicRepeatSampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_PointRepeatSampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearRepeatSampler;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_RepeatSampler;

layout(location = 0) in vec2 PS_IN_aStream;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0));
    _streams.aStream_id0 = PS_IN_aStream;
    vec2 uv2 = _streams.aStream_id0;
    vec4 f_noSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler, uv2);
    vec4 f_pointSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_PointSampler, uv2);
    vec4 f_linearSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearSampler, uv2);
    vec4 f_linearBorderSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearBorderSampler, uv2);
    vec4 f_linearClampCompareLessEqualSampler = vec4(texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearClampCompareLessEqualSampler, vec3(uv2, uv2.y)));
    vec4 f_anisotropicSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_AnisotropicSampler, uv2);
    vec4 f_anisotropicRepeatSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_AnisotropicRepeatSampler, uv2);
    vec4 f_pointRepeatSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_PointRepeatSampler, uv2);
    vec4 f_linearRepeatSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_LinearRepeatSampler, uv2);
    vec4 f_repeatSampler = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_RepeatSampler, uv2);
}

