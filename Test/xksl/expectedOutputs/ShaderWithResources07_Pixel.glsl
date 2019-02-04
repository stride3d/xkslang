#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform cb
{
    int ShaderMain_lightIndex;
} cb_var;

uniform usamplerBuffer ShaderMain_LightIndices;
uniform samplerBuffer ShaderMain_PointLights;
uniform usampler3D SPIRV_Cross_CombinedShaderMain_LightClustersSPIRV_Cross_DummySampler;

void main()
{
    int realLightIndex = int(texelFetch(ShaderMain_LightIndices, cb_var.ShaderMain_lightIndex).x);
    vec4 pointLight1 = texelFetch(ShaderMain_PointLights, realLightIndex * 2);
    uvec2 lightData = uvec2(texelFetch(SPIRV_Cross_CombinedShaderMain_LightClustersSPIRV_Cross_DummySampler, ivec4(0, 1, 2, 3).xyz, ivec4(0, 1, 2, 3).w).xy);
}

