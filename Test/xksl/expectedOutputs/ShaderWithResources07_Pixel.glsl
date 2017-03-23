#version 450

layout(std140) uniform cb
{
    vec3 ShaderA_direction;
    vec2 ShaderA_uv2;
    int ShaderB_lightIndex;
} cb_var;

uniform samplerCube ShaderB_LightIndices;
uniform samplerBuffer ShaderB_PointLights;

void main()
{
    int realLightIndex = int(texelFetch(ShaderB_LightIndices, cb_var.ShaderB_lightIndex.xyz).x);
    vec4 pointLight1 = texelFetch(ShaderB_PointLights, realLightIndex * 2);
    uvec2 lightData = uvec2(texelFetch(ShaderB_LightClusters, ivec4(0, 1, 2, 3).xyz, ivec4(0, 1, 2, 3).w).xy);
}

