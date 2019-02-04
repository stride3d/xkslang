#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 s_in_id0;
    vec4 s_int_id1;
};

layout(std140) uniform PerMaterial
{
    vec4 ShaderMain_scale;
    vec4 ShaderMain_offset;
    vec4 ShaderMain_aCol;
    vec2 ShaderMain_uv2;
} PerMaterial_var;

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;

in vec4 VS_IN_S_INPUT;
out vec4 VS_OUT_s_int;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.s_in_id0 = VS_IN_S_INPUT;
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, PerMaterial_var.ShaderMain_uv2);
    _streams.s_int_id1 = _streams.s_in_id0 + PerMaterial_var.ShaderMain_scale;
    VS_OUT_s_int = _streams.s_int_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

