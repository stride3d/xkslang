#version 450

layout(std140) uniform Globals
{
    vec3 ShaderMain_direction;
    vec2 ShaderMain_uv2;
} Globals_var;

uniform samplerCube SPIRV_Cross_CombinedShaderMain_CubeMapShaderMain_Sampler0;

void main()
{
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_CubeMapShaderMain_Sampler0, Globals_var.ShaderMain_direction);
}

