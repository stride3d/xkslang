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
    float3 meshNormalWS_id3;
    float3 normalWS_id4;
    float4 PositionWS_id5;
    float3 matNormal_id6;
    float4 matColorBase_id7;
    float4 matDiffuse_id8;
    float matGlossiness_id9;
    float3 matSpecular_id10;
    float matSpecularIntensity_id11;
    float matAmbientOcclusion_id12;
    float matAmbientOcclusionDirectLightingFactor_id13;
    float matCavity_id14;
    float matCavityDiffuse_id15;
    float matCavitySpecular_id16;
    float4 matEmissive_id17;
    float matEmissiveIntensity_id18;
    float matScatteringStrength_id19;
    float2 matDiffuseSpecularAlphaBlend_id20;
    float3 matAlphaBlendColor_id21;
    float matAlphaDiscard_id22;
    float3 viewWS_id23;
    float3 matDiffuseVisible_id24;
    float alphaRoughness_id25;
    float3 matSpecularVisible_id26;
    float NdotV_id27;
    float3 shadingColor_id28;
    float shadingColorAlpha_id29;
    float3 H_id30;
    float NdotH_id31;
    float LdotH_id32;
    float VdotH_id33;
    float3 lightPositionWS_id34;
    float3 lightDirectionWS_id35;
    float3 lightColor_id36;
    float3 lightColorNdotL_id37;
    float3 envLightDiffuseColor_id38;
    float3 envLightSpecularColor_id39;
    float NdotL_id40;
    float lightDirectAmbientOcclusion_id41;
    float matBlend_id42;
    float3 shadowColor_id43;
    float thicknessWS_id44;
    uint2 lightData_id45;
    int lightIndex_id46;
    float4 ScreenPosition_id47;
};

static const PS_STREAMS _1042 = { 0.0f.xxxx, false, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f.xxxx, 0.0f, 0.0f, 0.0f.xx, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f.xxx, 0.0f, uint2(0u, 0u), 0, 0.0f.xxxx };

cbuffer PerView
{
    column_major float4x4 Transformation_View;
    column_major float4x4 Transformation_ViewInverse;
    column_major float4x4 Transformation_Projection;
    column_major float4x4 Transformation_ProjectionInverse;
    column_major float4x4 Transformation_ViewProjection;
    float2 Transformation_ProjScreenRay;
    float4 Transformation_Eye;
    float o1S437C0_Camera_NearClipPlane;
    float o1S437C0_Camera_FarClipPlane;
    float2 o1S437C0_Camera_ZProjection;
    float2 o1S437C0_Camera_ViewSize;
    float o1S437C0_Camera_AspectRatio;
    float4 o0S437C0_LightDirectionalGroup_padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S437C0_LightDirectionalGroup_Lights[8];
    int o0S437C0_DirectLightGroupPerView_LightCount;
    float o1S437C0_LightClustered_ClusterDepthScale;
    float o1S437C0_LightClustered_ClusterDepthBias;
    float2 o1S437C0_LightClustered_ClusterStride;
    float3 o3S421C0_LightSimpleAmbient_AmbientLight;
    float4 o3S421C0_LightSimpleAmbient_padding_PerView_Lighting;
};
cbuffer PerMaterial
{
    float4 o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
};
Buffer<float4> LightClusteredPointGroup_PointLights;
Texture3D<uint4> LightClustered_LightClusters;
Buffer<uint4> LightClustered_LightIndices;
Buffer<float4> LightClusteredSpotGroup_SpotLights;

static float4 PS_IN_SV_Position;
static float3 PS_IN_NORMALWS;
static float4 PS_IN_POSITION_WS;
static float4 PS_IN_SCREENPOSITION;
static bool PS_IN_SV_IsFrontFace;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float3 PS_IN_NORMALWS : NORMALWS;
    float4 PS_IN_POSITION_WS : POSITION_WS;
    float4 PS_IN_SCREENPOSITION : SCREENPOSITION;
    bool PS_IN_SV_IsFrontFace : SV_IsFrontFace;
    float4 PS_IN_SV_Position : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id4, _streams.normalWS_id4) > 0.0f)
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
    _streams.matBlend_id42 = 0.0f;
}

void o9S251C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S251C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id6 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id7 = 0.0f.xxxx;
    _streams.matDiffuse_id8 = 0.0f.xxxx;
    _streams.matDiffuseVisible_id24 = 0.0f.xxx;
    _streams.matSpecular_id10 = 0.0f.xxx;
    _streams.matSpecularVisible_id26 = 0.0f.xxx;
    _streams.matSpecularIntensity_id11 = 1.0f;
    _streams.matGlossiness_id9 = 0.0f;
    _streams.alphaRoughness_id25 = 1.0f;
    _streams.matAmbientOcclusion_id12 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id13 = 0.0f;
    _streams.matCavity_id14 = 1.0f;
    _streams.matCavityDiffuse_id15 = 0.0f;
    _streams.matCavitySpecular_id16 = 0.0f;
    _streams.matEmissive_id17 = 0.0f.xxxx;
    _streams.matEmissiveIntensity_id18 = 0.0f;
    _streams.matScatteringStrength_id19 = 1.0f;
    _streams.matDiffuseSpecularAlphaBlend_id20 = 1.0f.xx;
    _streams.matAlphaBlendColor_id21 = 1.0f.xxx;
    _streams.matAlphaDiscard_id22 = 0.100000001490116119384765625f;
}

void o9S251C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o9S251C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id29 = 1.0f;
}

float4 o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_GizmoUniformColorMaterial_GizmoColorKey__Compute()
{
    return o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
}

void o8S251C0_o5S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o8S251C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_GizmoUniformColorMaterial_GizmoColorKey__Compute();
    _streams.matDiffuse_id8 = colorBase;
    _streams.matColorBase_id7 = colorBase;
}

void NormalFromMesh_UpdateNormalFromTangentSpace(float3 normalInTangentSpace)
{
}

void o8S251C0_o7S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id34 = 0.0f.xxx;
    _streams.lightDirectionWS_id35 = 0.0f.xxx;
    _streams.lightColor_id36 = 0.0f.xxx;
    _streams.lightColorNdotL_id37 = 0.0f.xxx;
    _streams.envLightDiffuseColor_id38 = 0.0f.xxx;
    _streams.envLightSpecularColor_id39 = 0.0f.xxx;
    _streams.lightDirectAmbientOcclusion_id41 = 1.0f;
    _streams.NdotL_id40 = 0.0f;
}

void o8S251C0_o7S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id41 = lerp(1.0f, _streams.matAmbientOcclusion_id12, _streams.matAmbientOcclusionDirectLightingFactor_id13);
    _streams.matDiffuseVisible_id24 = ((_streams.matDiffuse_id8.xyz * lerp(1.0f, _streams.matCavity_id14, _streams.matCavityDiffuse_id15)) * _streams.matDiffuseSpecularAlphaBlend_id20.x) * _streams.matAlphaBlendColor_id21;
    _streams.matSpecularVisible_id26 = (((_streams.matSpecular_id10 * _streams.matSpecularIntensity_id11) * lerp(1.0f, _streams.matCavity_id14, _streams.matCavitySpecular_id16)) * _streams.matDiffuseSpecularAlphaBlend_id20.y) * _streams.matAlphaBlendColor_id21;
    _streams.NdotV_id27 = max(dot(_streams.normalWS_id4, _streams.viewWS_id23), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id9;
    _streams.alphaRoughness_id25 = max(roughness * roughness, 0.001000000047497451305389404296875f);
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
    return o0S437C0_DirectLightGroupPerView_LightCount;
}

void o0S437C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id36 = o0S437C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id35 = -o0S437C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

float3 o0S437C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id44 = 0.0f;
    return 1.0f.xxx;
}

void o0S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S437C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id40 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id35), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    float3 _155 = o0S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id43 = _155;
    _streams.lightColorNdotL_id37 = ((_streams.lightColor_id36 * _streams.shadowColor_id43) * _streams.NdotL_id40) * _streams.lightDirectAmbientOcclusion_id41;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id30 = normalize(_streams.viewWS_id23 + _streams.lightDirectionWS_id35);
    _streams.NdotH_id31 = clamp(dot(_streams.normalWS_id4, _streams.H_id30), 0.0f, 1.0f);
    _streams.LdotH_id32 = clamp(dot(_streams.lightDirectionWS_id35, _streams.H_id30), 0.0f, 1.0f);
    _streams.VdotH_id33 = _streams.LdotH_id32;
}

float3 o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id24;
    if (false)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id26);
    }
    return ((diffuseColor / 3.1415927410125732421875f.xxx) * _streams.lightColorNdotL_id37) * _streams.matDiffuseSpecularAlphaBlend_id20.x;
}

void o1S437C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = o1S437C0_Camera_ZProjection.y / (projectedDepth - o1S437C0_Camera_ZProjection.x);
    float2 texCoord = float2(_streams.ScreenPosition_id47.x + 1.0f, 1.0f - _streams.ScreenPosition_id47.y) * 0.5f;
    int slice = int(max(log2((depth * o1S437C0_LightClustered_ClusterDepthScale) + o1S437C0_LightClustered_ClusterDepthBias), 0.0f));
    _streams.lightData_id45 = uint2(LightClustered_LightClusters.Load(int4(int4(int2(texCoord * o1S437C0_LightClustered_ClusterStride), slice, 0).xyz, int4(int2(texCoord * o1S437C0_LightClustered_ClusterStride), slice, 0).w)).xy);
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
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o1S437C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S437C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S437C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, float3 position, out float3 lightVectorNorm)
{
    float3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / lightVectorLength.xxx;
    float lightInvSquareRadius = light.InvSquareRadius;
    float3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S437C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S437C0_LightPoint_ProcessLight(inout PS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id5.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _300 = o1S437C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _300;
    _streams.lightPositionWS_id34 = light.PositionWS;
    _streams.lightColor_id36 = light.Color * attenuation;
    _streams.lightDirectionWS_id35 = lightVectorNorm;
}

void o1S437C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id46).x);
    _streams.lightIndex_id46++;
    float4 pointLight1 = LightClusteredPointGroup_PointLights.Load(realLightIndex * 2);
    float4 pointLight2 = LightClusteredPointGroup_PointLights.Load((realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S437C0_LightPoint_ProcessLight(_streams, param);
}

float3 o1S437C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id44 = 0.0f;
    return 1.0f.xxx;
}

void o1S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S437C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id40 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id35), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    float3 _216 = o1S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
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
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o2S437C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S437C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S437C0_LightUtil_GetAngleAttenuation(float3 lightVector, float3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0f, 1.0f);
    attenuation *= attenuation;
    return attenuation;
}

float o2S437C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, float3 position, inout float3 lightVectorNorm)
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
    attenuation *= o2S437C0_LightUtil_GetDistanceAttenuation(param, param_1);
    float3 param_2 = lightVectorNorm;
    float3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S437C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S437C0_LightSpot_ProcessLight(inout PS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id5.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _587 = o2S437C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _587;
    _streams.lightColor_id36 = light.Color * attenuation;
    _streams.lightDirectionWS_id35 = lightVectorNorm;
}

void o2S437C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id46).x);
    _streams.lightIndex_id46++;
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
    o2S437C0_LightSpot_ProcessLight(_streams, param);
}

float3 o2S437C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id44 = 0.0f;
    return 1.0f.xxx;
}

void o2S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S437C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id40 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id35), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    float3 _491 = o2S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id43 = _491;
    _streams.lightColorNdotL_id37 = ((_streams.lightColor_id36 * _streams.shadowColor_id43) * _streams.NdotL_id40) * _streams.lightDirectAmbientOcclusion_id41;
}

void o3S421C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id38 = 0.0f.xxx;
    _streams.envLightSpecularColor_id39 = 0.0f.xxx;
}

void o3S421C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S421C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float3 lightColor = o3S421C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id12;
    _streams.envLightDiffuseColor_id38 = lightColor;
    _streams.envLightSpecularColor_id39 = lightColor;
}

float3 o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id24;
    if (false)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id26);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id38;
}

void o8S251C0_o7S2C0_o6S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o8S251C0_o7S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id6;
    NormalFromMesh_UpdateNormalFromTangentSpace(param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id4 = -_streams.normalWS_id4;
    }
    o8S251C0_o7S2C0_LightStream_ResetLightStream(_streams);
    o8S251C0_o7S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o8S251C0_o7S2C0_o6S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    float3 directLightingContribution = 0.0f.xxx;
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
    float3 environmentLightingContribution = 0.0f.xxx;
    o3S421C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o8S251C0_o7S2C0_o6S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id28 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id29 = _streams.matDiffuse_id8.w;
    o8S251C0_o7S2C0_o6S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
}

void o8S251C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o8S251C0_o5S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o8S251C0_o7S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id23 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id5.xyz);
    _streams.shadingColor_id28 = 0.0f.xxx;
    o9S251C1_MaterialPixelShadingStream_ResetStream(_streams);
    o8S251C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id28, _streams.shadingColorAlpha_id29);
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
    PS_STREAMS _streams = _1042;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.normalWS_id4 = PS_IN_NORMALWS;
    _streams.PositionWS_id5 = PS_IN_POSITION_WS;
    _streams.ScreenPosition_id47 = PS_IN_SCREENPOSITION;
    _streams.IsFrontFace_id1 = PS_IN_SV_IsFrontFace;
    _streams.ScreenPosition_id47 /= _streams.ScreenPosition_id47.w.xxxx;
    NormalBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    PS_IN_NORMALWS = stage_input.PS_IN_NORMALWS;
    PS_IN_POSITION_WS = stage_input.PS_IN_POSITION_WS;
    PS_IN_SCREENPOSITION = stage_input.PS_IN_SCREENPOSITION;
    PS_IN_SV_IsFrontFace = stage_input.PS_IN_SV_IsFrontFace;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
