#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

uniform samplerBuffer ShaderMain_PointLights;
uniform samplerBuffer ShaderMain_SpotLights;
uniform usampler3D SPIRV_Cross_CombinedShaderMain_LightClustersSPIRV_Cross_DummySampler;

void main()
{
    vec2 texCoord = vec2(0.0);
    uvec2 lightData = uvec2(texelFetch(SPIRV_Cross_CombinedShaderMain_LightClustersSPIRV_Cross_DummySampler, ivec4(ivec2(texCoord), 0, 0).xyz, ivec4(ivec2(texCoord), 0, 0).w).xy);
    int realLightIndex = 1;
    vec4 pointLight1 = texelFetch(ShaderMain_PointLights, realLightIndex * 2);
    vec4 spotLight1 = texelFetch(ShaderMain_SpotLights, realLightIndex * 4);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

