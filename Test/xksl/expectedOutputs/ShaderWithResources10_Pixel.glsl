#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct ShaderMain_SpotLightDataInternal
{
    vec3 PositionWS;
    vec3 DirectionWS;
    vec3 AngleOffsetAndInvSquareRadius;
    vec3 Color;
};

struct PS_STREAMS
{
    uvec2 lightData_id0;
    int lightIndex_id1;
    vec4 screenPosition_id2;
};

uniform usamplerBuffer ShaderMain_LightIndices;
uniform samplerBuffer ShaderMain_SpotLights;

in int PS_IN_LIGHTINDEX;

void ShaderMain_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(ShaderMain_LightIndices, _streams.lightIndex_id1).x);
    _streams.lightIndex_id1++;
    vec4 spotLight1 = texelFetch(ShaderMain_SpotLights, realLightIndex * 4);
    vec4 spotLight2 = texelFetch(ShaderMain_SpotLights, (realLightIndex * 4) + 1);
    vec4 spotLight3 = texelFetch(ShaderMain_SpotLights, (realLightIndex * 4) + 2);
    vec4 spotLight4 = texelFetch(ShaderMain_SpotLights, (realLightIndex * 4) + 3);
    ShaderMain_SpotLightDataInternal spotLight;
    spotLight.PositionWS = spotLight1.xyz;
    spotLight.DirectionWS = spotLight2.xyz;
    spotLight.AngleOffsetAndInvSquareRadius = spotLight3.xyz;
    spotLight.Color = spotLight4.xyz;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(uvec2(0u), 0, vec4(0.0));
    _streams.lightIndex_id1 = PS_IN_LIGHTINDEX;
    _streams.lightData_id0 = uvec2(0u);
    _streams.screenPosition_id2 = vec4(1.0, 2.0, 3.0, 4.0);
    int param = 0;
    ShaderMain_PrepareDirectLightCore(_streams, param);
}

