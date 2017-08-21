#version 450

layout(std140) uniform cb
{
    int ShaderMain_lightIndex;
} cb_var;

uniform usamplerBuffer ShaderMain_LightIndices;
uniform samplerBuffer ShaderMain_PointLights;

void main()
{
    int realLightIndex = int(texelFetch(ShaderMain_LightIndices, cb_var.ShaderMain_lightIndex).x);
    vec4 pointLight1 = texelFetch(ShaderMain_PointLights, realLightIndex * 2);
    uvec2 lightData = uvec2(texelFetch(ShaderMain_LightClusters, ivec4(0, 1, 2, 3).xyz, ivec4(0, 1, 2, 3).w).xy);
}

