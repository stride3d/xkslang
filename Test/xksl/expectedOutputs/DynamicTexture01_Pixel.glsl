#version 450

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;
uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture1ShaderMain_Sampler1;

void main()
{
    vec4 color0 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, vec2(0.5));
    vec4 color1 = texture(SPIRV_Cross_CombinedShaderMain_Texture1ShaderMain_Sampler1, vec2(0.5));
}

