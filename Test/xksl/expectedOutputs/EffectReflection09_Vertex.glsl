#version 450

uniform samplerBuffer ShaderMain_PointLights;
uniform samplerBuffer ShaderMain_SpotLights;

void main()
{
    vec2 texCoord = vec2(0.0);
    uvec2 lightData = uvec2(texelFetch(ShaderMain_LightClusters, ivec4(ivec2(texCoord), 0, 0).xyz, ivec4(ivec2(texCoord), 0, 0).w).xy);
    int realLightIndex = 1;
    vec4 pointLight1 = texelFetch(ShaderMain_PointLights, realLightIndex * 2);
    vec4 spotLight1 = texelFetch(ShaderMain_SpotLights, realLightIndex * 4);
}

