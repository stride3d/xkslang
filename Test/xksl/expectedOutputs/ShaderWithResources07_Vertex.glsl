#version 450

layout(std140) uniform cb
{
    vec3 ShaderA_direction;
    vec2 ShaderA_uv2;
    int ShaderB_lightIndex;
} cb_var;

uniform sampler2D SPIRV_Cross_CombinedShaderA_Texture0ShaderA_Sampler0;
uniform samplerCube SPIRV_Cross_CombinedShaderA_CubeMapShaderA_Sampler0;

void main()
{
    vec4 color1 = texture(SPIRV_Cross_CombinedShaderA_Texture0ShaderA_Sampler0, cb_var.ShaderA_uv2);
    vec4 color2 = texture(SPIRV_Cross_CombinedShaderA_CubeMapShaderA_Sampler0, cb_var.ShaderA_direction);
}

