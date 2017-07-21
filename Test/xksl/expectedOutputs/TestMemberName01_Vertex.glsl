#version 450

struct VS_STREAMS
{
    vec4 s_in_id0;
    vec4 s_int_id1;
};

layout(std140) uniform UpdatedGroupName
{
    vec4 ShaderMain_ShadowMapTextureSize;
    vec4 ShaderMain_ShadowMapTextureTexelSize;
} UpdatedGroupName_var;

uniform sampler2D SPIRV_Cross_CombinedShaderMain_ShadowMapTextureShaderMain_Sampler0;

layout(location = 0) in vec4 VS_IN_s_in;
layout(location = 0) out vec4 VS_OUT_s_int;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.s_in_id0 = VS_IN_s_in;
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_ShadowMapTextureShaderMain_Sampler0, vec2(0.0));
    _streams.s_int_id1 = _streams.s_in_id0 + UpdatedGroupName_var.ShaderMain_ShadowMapTextureSize;
    VS_OUT_s_int = _streams.s_int_id1;
}

