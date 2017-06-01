#version 450

layout(std140) uniform cb
{
    vec3 ShaderMain_direction;
    vec2 ShaderMain_uv2;
} cb_var;

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;

void main()
{
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, cb_var.ShaderMain_uv2);
}

