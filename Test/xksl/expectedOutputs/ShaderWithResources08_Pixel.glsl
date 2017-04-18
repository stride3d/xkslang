#version 450

uniform samplerCube SPIRV_Cross_CombinedShaderMain_CubeMapShaderMain_Sampler0;

void main()
{
    vec3 direction = vec3(1.0, 0.0, 0.0);
    vec4 color = texture(SPIRV_Cross_CombinedShaderMain_CubeMapShaderMain_Sampler0, direction);
}

