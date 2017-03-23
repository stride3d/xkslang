#version 450

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;

void main()
{
    vec2 uv2 = vec2(0.5);
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, uv2);
}

