#version 450

layout(std140) uniform globalCbuffer
{
    vec3 ShaderMain_direction;
    vec2 ShaderMain_uv2;
} globalCbuffer_var;

uniform samplerCube SPIRV_Cross_CombinedShaderMain_CubeMapShaderMain_Sampler0;

void main()
{
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_CubeMapShaderMain_Sampler0, globalCbuffer_var.ShaderMain_direction);
}

