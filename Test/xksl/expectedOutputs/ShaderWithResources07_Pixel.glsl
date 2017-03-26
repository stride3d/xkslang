#version 450

layout(std140) uniform cb
{
    vec3 ShaderA_direction;
    vec2 ShaderA_uv2;
    int ShaderB_lightIndex;
} cb_var;

layout(r32ui) uniform readonly uimageBuffer ShaderB_LightIndices;
layout(rgba32f) uniform readonly imageBuffer ShaderB_PointLights;

void main()
{
    int realLightIndex = int(imageLoad(ShaderB_LightIndices, cb_var.ShaderB_lightIndex).x);
    vec4 pointLight1 = imageLoad(ShaderB_PointLights, realLightIndex * 2);
    uvec2 lightData = uvec2(texelFetch(ShaderB_LightClusters, ivec4(0, 1, 2, 3).xyz, ivec4(0, 1, 2, 3).w).xy);
}

