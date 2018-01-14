#version 450

struct LightDirectional_DirectionalLightData
{
    vec3 DirectionWS;
    vec3 Color;
};

struct LightPoint_PointLightDataInternal
{
    vec3 PositionWS;
    float InvSquareRadius;
    vec3 Color;
};

struct LightSpot_SpotLightDataInternal
{
    vec3 PositionWS;
    vec3 DirectionWS;
    vec3 AngleOffsetAndInvSquareRadius;
    vec3 Color;
};

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    bool IsFrontFace_id1;
    vec4 ColorTarget_id2;
    vec3 meshNormalWS_id3;
    vec3 normalWS_id4;
    vec4 PositionWS_id5;
    vec3 matNormal_id6;
    vec4 matColorBase_id7;
    vec4 matDiffuse_id8;
    float matGlossiness_id9;
    vec3 matSpecular_id10;
    float matSpecularIntensity_id11;
    float matAmbientOcclusion_id12;
    float matAmbientOcclusionDirectLightingFactor_id13;
    float matCavity_id14;
    float matCavityDiffuse_id15;
    float matCavitySpecular_id16;
    vec4 matEmissive_id17;
    float matEmissiveIntensity_id18;
    float matScatteringStrength_id19;
    vec2 matDiffuseSpecularAlphaBlend_id20;
    vec3 matAlphaBlendColor_id21;
    float matAlphaDiscard_id22;
    vec3 viewWS_id23;
    vec3 matDiffuseVisible_id24;
    float alphaRoughness_id25;
    vec3 matSpecularVisible_id26;
    float NdotV_id27;
    vec3 shadingColor_id28;
    float shadingColorAlpha_id29;
    vec3 H_id30;
    float NdotH_id31;
    float LdotH_id32;
    float VdotH_id33;
    vec3 lightPositionWS_id34;
    vec3 lightDirectionWS_id35;
    vec3 lightColor_id36;
    vec3 lightColorNdotL_id37;
    vec3 envLightDiffuseColor_id38;
    vec3 envLightSpecularColor_id39;
    float NdotL_id40;
    float lightDirectAmbientOcclusion_id41;
    float matBlend_id42;
    vec3 shadowColor_id43;
    float thicknessWS_id44;
    uvec2 lightData_id45;
    int lightIndex_id46;
    vec4 ScreenPosition_id47;
};

layout(std140) uniform PerView
{
    layout(row_major) mat4 Transformation_View;
    layout(row_major) mat4 Transformation_ViewInverse;
    layout(row_major) mat4 Transformation_Projection;
    layout(row_major) mat4 Transformation_ProjectionInverse;
    layout(row_major) mat4 Transformation_ViewProjection;
    vec2 Transformation_ProjScreenRay;
    vec4 Transformation_Eye;
    float o1S437C0_Camera_NearClipPlane;
    float o1S437C0_Camera_FarClipPlane;
    vec2 o1S437C0_Camera_ZProjection;
    vec2 o1S437C0_Camera_ViewSize;
    float o1S437C0_Camera_AspectRatio;
    vec4 o0S437C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S437C0_LightDirectionalGroup_Lights[8];
    int o0S437C0_DirectLightGroupPerView_LightCount;
    float o1S437C0_LightClustered_ClusterDepthScale;
    float o1S437C0_LightClustered_ClusterDepthBias;
    vec2 o1S437C0_LightClustered_ClusterStride;
    vec3 o3S421C0_LightSimpleAmbient_AmbientLight;
    vec4 o3S421C0_LightSimpleAmbient__padding_PerView_Lighting;
} PerView_var;

layout(std140) uniform PerMaterial
{
    vec4 o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
} PerMaterial_var;

uniform samplerBuffer LightClusteredPointGroup_PointLights;
uniform usamplerBuffer LightClustered_LightIndices;
uniform samplerBuffer LightClusteredSpotGroup_SpotLights;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_normalWS;
layout(location = 2) in vec4 PS_IN_PositionWS;
layout(location = 3) in vec4 PS_IN_ScreenPosition;
layout(location = 4) in bool PS_IN_IsFrontFace;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id4, _streams.normalWS_id4) > 0.0)
    {
        _streams.normalWS_id4 = normalize(_streams.normalWS_id4);
    }
    _streams.meshNormalWS_id3 = _streams.normalWS_id4;
}

void ShaderBase_PSMain()
{
}

void o9S251C1_IStreamInitializer_ResetStream()
{
}

void o9S251C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S251C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id42 = 0.0;
}

void o9S251C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S251C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id6 = vec3(0.0, 0.0, 1.0);
    _streams.matColorBase_id7 = vec4(0.0);
    _streams.matDiffuse_id8 = vec4(0.0);
    _streams.matDiffuseVisible_id24 = vec3(0.0);
    _streams.matSpecular_id10 = vec3(0.0);
    _streams.matSpecularVisible_id26 = vec3(0.0);
    _streams.matSpecularIntensity_id11 = 1.0;
    _streams.matGlossiness_id9 = 0.0;
    _streams.alphaRoughness_id25 = 1.0;
    _streams.matAmbientOcclusion_id12 = 1.0;
    _streams.matAmbientOcclusionDirectLightingFactor_id13 = 0.0;
    _streams.matCavity_id14 = 1.0;
    _streams.matCavityDiffuse_id15 = 0.0;
    _streams.matCavitySpecular_id16 = 0.0;
    _streams.matEmissive_id17 = vec4(0.0);
    _streams.matEmissiveIntensity_id18 = 0.0;
    _streams.matScatteringStrength_id19 = 1.0;
    _streams.matDiffuseSpecularAlphaBlend_id20 = vec2(1.0);
    _streams.matAlphaBlendColor_id21 = vec3(1.0);
    _streams.matAlphaDiscard_id22 = 0.100000001490116119384765625;
}

void o9S251C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S251C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id29 = 1.0;
}

vec4 o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_GizmoUniformColorMaterial_GizmoColorKey__Compute()
{
    return PerMaterial_var.o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
}

void o8S251C0_o5S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_GizmoUniformColorMaterial_GizmoColorKey__Compute();
    _streams.matDiffuse_id8 = colorBase;
    _streams.matColorBase_id7 = colorBase;
}

void NormalFromMesh_UpdateNormalFromTangentSpace(vec3 normalInTangentSpace)
{
}

void o8S251C0_o7S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id34 = vec3(0.0);
    _streams.lightDirectionWS_id35 = vec3(0.0);
    _streams.lightColor_id36 = vec3(0.0);
    _streams.lightColorNdotL_id37 = vec3(0.0);
    _streams.envLightDiffuseColor_id38 = vec3(0.0);
    _streams.envLightSpecularColor_id39 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id41 = 1.0;
    _streams.NdotL_id40 = 0.0;
}

void o8S251C0_o7S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id41 = mix(1.0, _streams.matAmbientOcclusion_id12, _streams.matAmbientOcclusionDirectLightingFactor_id13);
    _streams.matDiffuseVisible_id24 = ((_streams.matDiffuse_id8.xyz * mix(1.0, _streams.matCavity_id14, _streams.matCavityDiffuse_id15)) * _streams.matDiffuseSpecularAlphaBlend_id20.x) * _streams.matAlphaBlendColor_id21;
    _streams.matSpecularVisible_id26 = (((_streams.matSpecular_id10 * _streams.matSpecularIntensity_id11) * mix(1.0, _streams.matCavity_id14, _streams.matCavitySpecular_id16)) * _streams.matDiffuseSpecularAlphaBlend_id20.y) * _streams.matAlphaBlendColor_id21;
    _streams.NdotV_id27 = max(dot(_streams.normalWS_id4, _streams.viewWS_id23), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id9;
    _streams.alphaRoughness_id25 = max(roughness * roughness, 0.001000000047497451305389404296875);
}

void o8S251C0_o7S2C0_o6S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o0S437C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S437C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S437C0_DirectLightGroupPerView_GetLightCount()
{
    return PerView_var.o0S437C0_DirectLightGroupPerView_LightCount;
}

void o0S437C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id36 = PerView_var.o0S437C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id35 = -PerView_var.o0S437C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

vec3 o0S437C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id44 = 0.0;
    return vec3(1.0);
}

void o0S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S437C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id40 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id35), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    vec3 _155 = o0S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id43 = _155;
    _streams.lightColorNdotL_id37 = ((_streams.lightColor_id36 * _streams.shadowColor_id43) * _streams.NdotL_id40) * _streams.lightDirectAmbientOcclusion_id41;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id30 = normalize(_streams.viewWS_id23 + _streams.lightDirectionWS_id35);
    _streams.NdotH_id31 = clamp(dot(_streams.normalWS_id4, _streams.H_id30), 0.0, 1.0);
    _streams.LdotH_id32 = clamp(dot(_streams.lightDirectionWS_id35, _streams.H_id30), 0.0, 1.0);
    _streams.VdotH_id33 = _streams.LdotH_id32;
}

vec3 o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id24;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id26);
    }
    return ((diffuseColor / vec3(3.1415927410125732421875)) * _streams.lightColorNdotL_id37) * _streams.matDiffuseSpecularAlphaBlend_id20.x;
}

void o1S437C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = PerView_var.o1S437C0_Camera_ZProjection.y / (projectedDepth - PerView_var.o1S437C0_Camera_ZProjection.x);
    vec2 texCoord = vec2(_streams.ScreenPosition_id47.x + 1.0, 1.0 - _streams.ScreenPosition_id47.y) * 0.5;
    int slice = int(max(log2((depth * PerView_var.o1S437C0_LightClustered_ClusterDepthScale) + PerView_var.o1S437C0_LightClustered_ClusterDepthBias), 0.0));
    _streams.lightData_id45 = uvec2(texelFetch(LightClustered_LightClusters, ivec4(ivec2(texCoord * PerView_var.o1S437C0_LightClustered_ClusterStride), slice, 0).xyz, ivec4(ivec2(texCoord * PerView_var.o1S437C0_LightClustered_ClusterStride), slice, 0).w).xy);
    _streams.lightIndex_id46 = int(_streams.lightData_id45.x);
}

void o1S437C0_LightClusteredPointGroup_PrepareDirectLights(inout PS_STREAMS _streams)
{
    o1S437C0_LightClustered_PrepareLightData(_streams);
}

int o1S437C0_LightClusteredPointGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id45.y & 65535u);
}

int o1S437C0_LightClusteredPointGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id45.y & 65535u);
}

float o1S437C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o1S437C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S437C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S437C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, vec3 position, out vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    float lightInvSquareRadius = light.InvSquareRadius;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S437C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S437C0_LightPoint_ProcessLight(inout PS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id5.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _300 = o1S437C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _300;
    _streams.lightPositionWS_id34 = light.PositionWS;
    _streams.lightColor_id36 = light.Color * attenuation;
    _streams.lightDirectionWS_id35 = lightVectorNorm;
}

void o1S437C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id46).x);
    _streams.lightIndex_id46++;
    vec4 pointLight1 = texelFetch(LightClusteredPointGroup_PointLights, realLightIndex * 2);
    vec4 pointLight2 = texelFetch(LightClusteredPointGroup_PointLights, (realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S437C0_LightPoint_ProcessLight(_streams, param);
}

vec3 o1S437C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id44 = 0.0;
    return vec3(1.0);
}

void o1S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S437C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id40 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id35), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    vec3 _216 = o1S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id43 = _216;
    _streams.lightColorNdotL_id37 = ((_streams.lightColor_id36 * _streams.shadowColor_id43) * _streams.NdotL_id40) * _streams.lightDirectAmbientOcclusion_id41;
}

void o2S437C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S437C0_LightClusteredSpotGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id45.y >> uint(16));
}

int o2S437C0_LightClusteredSpotGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id45.y >> uint(16));
}

float o2S437C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o2S437C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S437C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S437C0_LightUtil_GetAngleAttenuation(vec3 lightVector, vec3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0, 1.0);
    attenuation *= attenuation;
    return attenuation;
}

float o2S437C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, vec3 position, inout vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    vec3 lightAngleOffsetAndInvSquareRadius = light.AngleOffsetAndInvSquareRadius;
    vec2 lightAngleAndOffset = lightAngleOffsetAndInvSquareRadius.xy;
    float lightInvSquareRadius = lightAngleOffsetAndInvSquareRadius.z;
    vec3 lightDirection = -light.DirectionWS;
    float attenuation = 1.0;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S437C0_LightUtil_GetDistanceAttenuation(param, param_1);
    vec3 param_2 = lightVectorNorm;
    vec3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S437C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S437C0_LightSpot_ProcessLight(inout PS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id5.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _587 = o2S437C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _587;
    _streams.lightColor_id36 = light.Color * attenuation;
    _streams.lightDirectionWS_id35 = lightVectorNorm;
}

void o2S437C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id46).x);
    _streams.lightIndex_id46++;
    vec4 spotLight1 = texelFetch(LightClusteredSpotGroup_SpotLights, realLightIndex * 4);
    vec4 spotLight2 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 1);
    vec4 spotLight3 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 2);
    vec4 spotLight4 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 3);
    LightSpot_SpotLightDataInternal spotLight;
    spotLight.PositionWS = spotLight1.xyz;
    spotLight.DirectionWS = spotLight2.xyz;
    spotLight.AngleOffsetAndInvSquareRadius = spotLight3.xyz;
    spotLight.Color = spotLight4.xyz;
    LightSpot_SpotLightDataInternal param = spotLight;
    o2S437C0_LightSpot_ProcessLight(_streams, param);
}

vec3 o2S437C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id44 = 0.0;
    return vec3(1.0);
}

void o2S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S437C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id40 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id35), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    vec3 _491 = o2S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id43 = _491;
    _streams.lightColorNdotL_id37 = ((_streams.lightColor_id36 * _streams.shadowColor_id43) * _streams.NdotL_id40) * _streams.lightDirectAmbientOcclusion_id41;
}

void o3S421C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id38 = vec3(0.0);
    _streams.envLightSpecularColor_id39 = vec3(0.0);
}

void o3S421C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S421C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    vec3 lightColor = PerView_var.o3S421C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id12;
    _streams.envLightDiffuseColor_id38 = lightColor;
    _streams.envLightSpecularColor_id39 = lightColor;
}

vec3 o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id24;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id26);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id38;
}

void o8S251C0_o7S2C0_o6S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o8S251C0_o7S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id6;
    NormalFromMesh_UpdateNormalFromTangentSpace(param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id4 = -_streams.normalWS_id4;
    }
    o8S251C0_o7S2C0_LightStream_ResetLightStream(_streams);
    o8S251C0_o7S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o8S251C0_o7S2C0_o6S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    vec3 directLightingContribution = vec3(0.0);
    o0S437C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S437C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S437C0_DirectLightGroupPerView_GetLightCount();
    int i = 0;
    int param_1;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o0S437C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
    }
    o1S437C0_LightClusteredPointGroup_PrepareDirectLights(_streams);
    maxLightCount = o1S437C0_LightClusteredPointGroup_GetMaxLightCount(_streams);
    count = o1S437C0_LightClusteredPointGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o1S437C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
    }
    o2S437C0_DirectLightGroup_PrepareDirectLights();
    maxLightCount = o2S437C0_LightClusteredSpotGroup_GetMaxLightCount(_streams);
    count = o2S437C0_LightClusteredSpotGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o2S437C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
    }
    vec3 environmentLightingContribution = vec3(0.0);
    o3S421C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id28 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id29 = _streams.matDiffuse_id8.w;
    o8S251C0_o7S2C0_o6S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
}

void o8S251C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o8S251C0_o5S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o8S251C0_o7S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id23 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id5.xyz);
    _streams.shadingColor_id28 = vec3(0.0);
    o9S251C1_MaterialPixelShadingStream_ResetStream(_streams);
    o8S251C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id28, _streams.shadingColorAlpha_id29);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _13 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void NormalBase_PSMain(inout PS_STREAMS _streams)
{
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), false, vec4(0.0), vec3(0.0), vec3(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), 0.0, uvec2(0u), 0, vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id4 = PS_IN_normalWS;
    _streams.PositionWS_id5 = PS_IN_PositionWS;
    _streams.ScreenPosition_id47 = PS_IN_ScreenPosition;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    _streams.ScreenPosition_id47 /= vec4(_streams.ScreenPosition_id47.w);
    NormalBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

