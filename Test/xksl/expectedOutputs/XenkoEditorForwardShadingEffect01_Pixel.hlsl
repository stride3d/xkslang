struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct LightPoint_PointLightDataInternal
{
    float3 PositionWS;
    float InvSquareRadius;
    float3 Color;
};

struct LightSpot_SpotLightDataInternal
{
    float3 PositionWS;
    float3 DirectionWS;
    float3 AngleOffsetAndInvSquareRadius;
    float3 Color;
};

struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    bool IsFrontFace_id1;
    float4 ColorTarget_id2;
    float3 normalWS_id3;
    float4 PositionWS_id4;
    float3 matNormal_id5;
    float4 matColorBase_id6;
    float4 matDiffuse_id7;
    float matGlossiness_id8;
    float3 matSpecular_id9;
    float matSpecularIntensity_id10;
    float matAmbientOcclusion_id11;
    float matAmbientOcclusionDirectLightingFactor_id12;
    float matCavity_id13;
    float matCavityDiffuse_id14;
    float matCavitySpecular_id15;
    float4 matEmissive_id16;
    float matEmissiveIntensity_id17;
    float2 matDiffuseSpecularAlphaBlend_id18;
    float3 matAlphaBlendColor_id19;
    float matAlphaDiscard_id20;
    float3 viewWS_id21;
    float3 matDiffuseVisible_id22;
    float alphaRoughness_id23;
    float3 matSpecularVisible_id24;
    float NdotV_id25;
    float3 shadingColor_id26;
    float shadingColorAlpha_id27;
    float3 H_id28;
    float NdotH_id29;
    float LdotH_id30;
    float VdotH_id31;
    float3 lightPositionWS_id32;
    float3 lightDirectionWS_id33;
    float3 lightColor_id34;
    float3 lightColorNdotL_id35;
    float3 envLightDiffuseColor_id36;
    float3 envLightSpecularColor_id37;
    float NdotL_id38;
    float lightDirectAmbientOcclusion_id39;
    float matBlend_id40;
    float3 shadowColor_id41;
    uint2 lightData_id42;
    int lightIndex_id43;
    float4 ScreenPosition_id44;
};

cbuffer PerView
{
    column_major float4x4 Transformation_View;
    column_major float4x4 Transformation_ViewInverse;
    column_major float4x4 Transformation_Projection;
    column_major float4x4 Transformation_ProjectionInverse;
    column_major float4x4 Transformation_ViewProjection;
    float2 Transformation_ProjScreenRay;
    float4 Transformation_Eye;
    float o1S429C0_Camera_NearClipPlane;
    float o1S429C0_Camera_FarClipPlane;
    float2 o1S429C0_Camera_ZProjection;
    float2 o1S429C0_Camera_ViewSize;
    float o1S429C0_Camera_AspectRatio;
    float4 o0S429C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S429C0_LightDirectionalGroup_Lights[8];
    int o0S429C0_DirectLightGroupPerView_LightCount;
    float o1S429C0_LightClustered_ClusterDepthScale;
    float o1S429C0_LightClustered_ClusterDepthBias;
    float2 o1S429C0_LightClustered_ClusterStride;
    float3 o3S414C0_LightSimpleAmbient_AmbientLight;
    float4 o3S414C0_LightSimpleAmbient__padding_PerView_Lighting;
};
cbuffer PerMaterial
{
    float4 o8S245C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
};
Buffer<float4> LightClusteredPointGroup_PointLights;
Texture3D<uint4> LightClustered_LightClusters;
Buffer<uint4> LightClustered_LightIndices;
Buffer<float4> LightClusteredSpotGroup_SpotLights;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_normalWS;
static float4 PS_IN_PositionWS;
static float4 PS_IN_ScreenPosition;
static bool PS_IN_IsFrontFace;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_normalWS : NORMALWS;
    float4 PS_IN_PositionWS : POSITION_WS;
    float4 PS_IN_ScreenPosition : SCREENPOSITION;
    bool PS_IN_IsFrontFace : SV_IsFrontFace;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id3, _streams.normalWS_id3) > 0.0f)
    {
        _streams.normalWS_id3 = normalize(_streams.normalWS_id3);
    }
}

void ShaderBase_PSMain()
{
}

void o9S245C1_IStreamInitializer_ResetStream()
{
}

void o9S245C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S245C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id40 = 0.0f;
}

void o9S245C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S245C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id5 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id6 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuse_id7 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuseVisible_id22 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecular_id9 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularVisible_id24 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularIntensity_id10 = 1.0f;
    _streams.matGlossiness_id8 = 0.0f;
    _streams.alphaRoughness_id23 = 1.0f;
    _streams.matAmbientOcclusion_id11 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id12 = 0.0f;
    _streams.matCavity_id13 = 1.0f;
    _streams.matCavityDiffuse_id14 = 0.0f;
    _streams.matCavitySpecular_id15 = 0.0f;
    _streams.matEmissive_id16 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matEmissiveIntensity_id17 = 0.0f;
    _streams.matDiffuseSpecularAlphaBlend_id18 = float2(1.0f, 1.0f);
    _streams.matAlphaBlendColor_id19 = float3(1.0f, 1.0f, 1.0f);
    _streams.matAlphaDiscard_id20 = 0.100000001490116119384765625f;
}

void o9S245C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S245C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id27 = 1.0f;
}

float4 o8S245C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_GizmoUniformColorMaterial_GizmoColorKey__Compute()
{
    return o8S245C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
}

void o8S245C0_o5S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o8S245C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_GizmoUniformColorMaterial_GizmoColorKey__Compute();
    _streams.matDiffuse_id7 = colorBase;
    _streams.matColorBase_id6 = colorBase;
}

void NormalFromMesh_UpdateNormalFromTangentSpace(float3 normalInTangentSpace)
{
}

void o8S245C0_o7S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id32 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectionWS_id33 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColor_id34 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColorNdotL_id35 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightDiffuseColor_id36 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id37 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectAmbientOcclusion_id39 = 1.0f;
    _streams.NdotL_id38 = 0.0f;
}

void o8S245C0_o7S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id39 = lerp(1.0f, _streams.matAmbientOcclusion_id11, _streams.matAmbientOcclusionDirectLightingFactor_id12);
    _streams.matDiffuseVisible_id22 = ((_streams.matDiffuse_id7.xyz * lerp(1.0f, _streams.matCavity_id13, _streams.matCavityDiffuse_id14)) * _streams.matDiffuseSpecularAlphaBlend_id18.x) * _streams.matAlphaBlendColor_id19;
    _streams.matSpecularVisible_id24 = (((_streams.matSpecular_id9 * _streams.matSpecularIntensity_id10) * lerp(1.0f, _streams.matCavity_id13, _streams.matCavitySpecular_id15)) * _streams.matDiffuseSpecularAlphaBlend_id18.y) * _streams.matAlphaBlendColor_id19;
    _streams.NdotV_id25 = max(dot(_streams.normalWS_id3, _streams.viewWS_id21), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id8;
    _streams.alphaRoughness_id23 = max(roughness * roughness, 0.001000000047497451305389404296875f);
}

void o0S429C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S429C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S429C0_DirectLightGroupPerView_GetLightCount()
{
    return o0S429C0_DirectLightGroupPerView_LightCount;
}

void o0S429C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id34 = o0S429C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id33 = -o0S429C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

float3 o0S429C0_ShadowGroup_ComputeShadow(float3 position, int lightIndex)
{
    return float3(1.0f, 1.0f, 1.0f);
}

void o0S429C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S429C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id38 = max(dot(_streams.normalWS_id3, _streams.lightDirectionWS_id33), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id4.xyz;
    int param_2 = lightIndex;
    _streams.shadowColor_id41 = o0S429C0_ShadowGroup_ComputeShadow(param_1, param_2);
    _streams.lightColorNdotL_id35 = ((_streams.lightColor_id34 * _streams.shadowColor_id41) * _streams.NdotL_id38) * _streams.lightDirectAmbientOcclusion_id39;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id28 = normalize(_streams.viewWS_id21 + _streams.lightDirectionWS_id33);
    _streams.NdotH_id29 = clamp(dot(_streams.normalWS_id3, _streams.H_id28), 0.0f, 1.0f);
    _streams.LdotH_id30 = clamp(dot(_streams.lightDirectionWS_id33, _streams.H_id28), 0.0f, 1.0f);
    _streams.VdotH_id31 = _streams.LdotH_id30;
}

float3 o8S245C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id22;
    if (false)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id24);
    }
    return ((diffuseColor / 3.1415927410125732421875f.xxx) * _streams.lightColorNdotL_id35) * _streams.matDiffuseSpecularAlphaBlend_id18.x;
}

void o1S429C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = o1S429C0_Camera_ZProjection.y / (projectedDepth - o1S429C0_Camera_ZProjection.x);
    float2 texCoord = float2(_streams.ScreenPosition_id44.x + 1.0f, 1.0f - _streams.ScreenPosition_id44.y) * 0.5f;
    int slice = int(max(log2((depth * o1S429C0_LightClustered_ClusterDepthScale) + o1S429C0_LightClustered_ClusterDepthBias), 0.0f));
    _streams.lightData_id42 = uint2(LightClustered_LightClusters.Load(int4(int4(int2(texCoord * o1S429C0_LightClustered_ClusterStride), slice, 0).xyz, int4(int2(texCoord * o1S429C0_LightClustered_ClusterStride), slice, 0).w)).xy);
    _streams.lightIndex_id43 = int(_streams.lightData_id42.x);
}

void o1S429C0_LightClusteredPointGroup_PrepareDirectLights(inout PS_STREAMS _streams)
{
    o1S429C0_LightClustered_PrepareLightData(_streams);
}

int o1S429C0_LightClusteredPointGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id42.y & 65535u);
}

int o1S429C0_LightClusteredPointGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id42.y & 65535u);
}

float o1S429C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o1S429C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S429C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S429C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, float3 position, out float3 lightVectorNorm)
{
    float3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / lightVectorLength.xxx;
    float lightInvSquareRadius = light.InvSquareRadius;
    float3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S429C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S429C0_LightPoint_ProcessLight(inout PS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id4.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _295 = o1S429C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _295;
    _streams.lightPositionWS_id32 = light.PositionWS;
    _streams.lightColor_id34 = light.Color * attenuation;
    _streams.lightDirectionWS_id33 = lightVectorNorm;
}

void o1S429C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id43).x);
    _streams.lightIndex_id43++;
    float4 pointLight1 = LightClusteredPointGroup_PointLights.Load(realLightIndex * 2);
    float4 pointLight2 = LightClusteredPointGroup_PointLights.Load((realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S429C0_LightPoint_ProcessLight(_streams, param);
}

float3 o1S429C0_ShadowGroup_ComputeShadow(float3 position, int lightIndex)
{
    return float3(1.0f, 1.0f, 1.0f);
}

void o1S429C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S429C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id38 = max(dot(_streams.normalWS_id3, _streams.lightDirectionWS_id33), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id4.xyz;
    int param_2 = lightIndex;
    _streams.shadowColor_id41 = o1S429C0_ShadowGroup_ComputeShadow(param_1, param_2);
    _streams.lightColorNdotL_id35 = ((_streams.lightColor_id34 * _streams.shadowColor_id41) * _streams.NdotL_id38) * _streams.lightDirectAmbientOcclusion_id39;
}

void o2S429C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S429C0_LightClusteredSpotGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id42.y >> uint(16));
}

int o2S429C0_LightClusteredSpotGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id42.y >> uint(16));
}

float o2S429C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o2S429C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S429C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S429C0_LightUtil_GetAngleAttenuation(float3 lightVector, float3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0f, 1.0f);
    attenuation *= attenuation;
    return attenuation;
}

float o2S429C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, float3 position, inout float3 lightVectorNorm)
{
    float3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / lightVectorLength.xxx;
    float3 lightAngleOffsetAndInvSquareRadius = light.AngleOffsetAndInvSquareRadius;
    float2 lightAngleAndOffset = lightAngleOffsetAndInvSquareRadius.xy;
    float lightInvSquareRadius = lightAngleOffsetAndInvSquareRadius.z;
    float3 lightDirection = -light.DirectionWS;
    float attenuation = 1.0f;
    float3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S429C0_LightUtil_GetDistanceAttenuation(param, param_1);
    float3 param_2 = lightVectorNorm;
    float3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S429C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S429C0_LightSpot_ProcessLight(inout PS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id4.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _581 = o2S429C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _581;
    _streams.lightColor_id34 = light.Color * attenuation;
    _streams.lightDirectionWS_id33 = lightVectorNorm;
}

void o2S429C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id43).x);
    _streams.lightIndex_id43++;
    float4 spotLight1 = LightClusteredSpotGroup_SpotLights.Load(realLightIndex * 4);
    float4 spotLight2 = LightClusteredSpotGroup_SpotLights.Load((realLightIndex * 4) + 1);
    float4 spotLight3 = LightClusteredSpotGroup_SpotLights.Load((realLightIndex * 4) + 2);
    float4 spotLight4 = LightClusteredSpotGroup_SpotLights.Load((realLightIndex * 4) + 3);
    LightSpot_SpotLightDataInternal spotLight;
    spotLight.PositionWS = spotLight1.xyz;
    spotLight.DirectionWS = spotLight2.xyz;
    spotLight.AngleOffsetAndInvSquareRadius = spotLight3.xyz;
    spotLight.Color = spotLight4.xyz;
    LightSpot_SpotLightDataInternal param = spotLight;
    o2S429C0_LightSpot_ProcessLight(_streams, param);
}

float3 o2S429C0_ShadowGroup_ComputeShadow(float3 position, int lightIndex)
{
    return float3(1.0f, 1.0f, 1.0f);
}

void o2S429C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S429C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id38 = max(dot(_streams.normalWS_id3, _streams.lightDirectionWS_id33), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id4.xyz;
    int param_2 = lightIndex;
    _streams.shadowColor_id41 = o2S429C0_ShadowGroup_ComputeShadow(param_1, param_2);
    _streams.lightColorNdotL_id35 = ((_streams.lightColor_id34 * _streams.shadowColor_id41) * _streams.NdotL_id38) * _streams.lightDirectAmbientOcclusion_id39;
}

void o3S414C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id36 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id37 = float3(0.0f, 0.0f, 0.0f);
}

void o3S414C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S414C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float3 lightColor = o3S414C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id11;
    _streams.envLightDiffuseColor_id36 = lightColor;
    _streams.envLightSpecularColor_id37 = lightColor;
}

float3 o8S245C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id22;
    if (false)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id24);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id36;
}

void o8S245C0_o7S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id5;
    NormalFromMesh_UpdateNormalFromTangentSpace(param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id3 = -_streams.normalWS_id3;
    }
    o8S245C0_o7S2C0_LightStream_ResetLightStream(_streams);
    o8S245C0_o7S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o0S429C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S429C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S429C0_DirectLightGroupPerView_GetLightCount();
    int i = 0;
    int param_1;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o0S429C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o8S245C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
    }
    o1S429C0_LightClusteredPointGroup_PrepareDirectLights(_streams);
    maxLightCount = o1S429C0_LightClusteredPointGroup_GetMaxLightCount(_streams);
    count = o1S429C0_LightClusteredPointGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o1S429C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o8S245C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
    }
    o2S429C0_DirectLightGroup_PrepareDirectLights();
    maxLightCount = o2S429C0_LightClusteredSpotGroup_GetMaxLightCount(_streams);
    count = o2S429C0_LightClusteredSpotGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o2S429C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o8S245C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
    }
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o3S414C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o8S245C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id26 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id27 = _streams.matDiffuse_id7.w;
}

void o8S245C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o8S245C0_o5S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o8S245C0_o7S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id21 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id4.xyz);
    _streams.shadingColor_id26 = float3(0.0f, 0.0f, 0.0f);
    o9S245C1_MaterialPixelShadingStream_ResetStream(_streams);
    o8S245C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _13 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void NormalBase_PSMain(inout PS_STREAMS _streams)
{
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), false, float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), uint2(0u, 0u), 0, float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id3 = PS_IN_normalWS;
    _streams.PositionWS_id4 = PS_IN_PositionWS;
    _streams.ScreenPosition_id44 = PS_IN_ScreenPosition;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    _streams.ScreenPosition_id44 /= _streams.ScreenPosition_id44.w.xxxx;
    NormalBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_normalWS = stage_input.PS_IN_normalWS;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_ScreenPosition = stage_input.PS_IN_ScreenPosition;
    PS_IN_IsFrontFace = stage_input.PS_IN_IsFrontFace;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
