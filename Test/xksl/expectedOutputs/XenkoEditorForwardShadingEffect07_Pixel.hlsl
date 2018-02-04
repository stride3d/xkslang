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
    float DepthVS_id6;
    float3 matNormal_id7;
    float4 matColorBase_id8;
    float4 matDiffuse_id9;
    float matGlossiness_id10;
    float3 matSpecular_id11;
    float matSpecularIntensity_id12;
    float matAmbientOcclusion_id13;
    float matAmbientOcclusionDirectLightingFactor_id14;
    float matCavity_id15;
    float matCavityDiffuse_id16;
    float matCavitySpecular_id17;
    float4 matEmissive_id18;
    float matEmissiveIntensity_id19;
    float matScatteringStrength_id20;
    float2 matDiffuseSpecularAlphaBlend_id21;
    float3 matAlphaBlendColor_id22;
    float matAlphaDiscard_id23;
    float3 viewWS_id24;
    float3 matDiffuseVisible_id25;
    float alphaRoughness_id26;
    float3 matSpecularVisible_id27;
    float NdotV_id28;
    float3 shadingColor_id29;
    float shadingColorAlpha_id30;
    float3 H_id31;
    float NdotH_id32;
    float LdotH_id33;
    float VdotH_id34;
    float3 lightPositionWS_id35;
    float3 lightDirectionWS_id36;
    float3 lightColor_id37;
    float3 lightColorNdotL_id38;
    float3 envLightDiffuseColor_id39;
    float3 envLightSpecularColor_id40;
    float NdotL_id41;
    float lightDirectAmbientOcclusion_id42;
    float matBlend_id43;
    float3 shadowColor_id44;
    float thicknessWS_id45;
    float2 TexCoord_id46;
    uint2 lightData_id47;
    int lightIndex_id48;
    float4 ScreenPosition_id49;
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
    float o1S437C0_Camera_NearClipPlane;
    float o1S437C0_Camera_FarClipPlane;
    float2 o1S437C0_Camera_ZProjection;
    float2 o1S437C0_Camera_ViewSize;
    float o1S437C0_Camera_AspectRatio;
    float4 o0S437C0_ShadowMapReceiverDirectional__padding_PerView_Default;
    float o0S437C0_ShadowMapReceiverDirectional_CascadeDepthSplits[4];
    LightDirectional_DirectionalLightData o0S437C0_LightDirectionalGroup_Lights[1];
    int o0S437C0_DirectLightGroupPerView_LightCount;
    float o1S437C0_LightClustered_ClusterDepthScale;
    float o1S437C0_LightClustered_ClusterDepthBias;
    float2 o1S437C0_LightClustered_ClusterStride;
    float3 o3S421C0_LightSimpleAmbient_AmbientLight;
    column_major float4x4 o6S421C0_LightSkyboxShader_SkyMatrix;
    float o6S421C0_LightSkyboxShader_Intensity;
    float4 o6S421C0_LightSkyboxShader__padding_PerView_Lighting;
};
cbuffer PerView_Lighting
{
    column_major float4x4 o0S437C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[4];
    column_major float4x4 o0S437C0_ShadowMapReceiverBase_InverseWorldToShadowCascadeUV[4];
    column_major float4x4 o0S437C0_ShadowMapReceiverBase_ViewMatrices[4];
    float2 o0S437C0_ShadowMapReceiverBase_DepthRanges[4];
    float o0S437C0_ShadowMapReceiverBase_DepthBiases[1];
    float o0S437C0_ShadowMapReceiverBase_OffsetScales[1];
    float2 o0S437C0_ShadowMapCommon_ShadowMapTextureSize;
    float2 o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize;
};
cbuffer PerMaterial
{
    float o23S251C0_o15S2C0_o14S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o23S251C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat;
    float4 o23S251C0_o11S2C0_o10S2C0_o9S2C1_ComputeColorConstantColorLink_constantColor;
    float2 o23S251C0_o11S2C0_o10S2C0_o8S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o23S251C0_o11S2C0_o10S2C0_o8S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
};
Buffer<float4> LightClusteredPointGroup_PointLights;
Texture3D<uint4> LightClustered_LightClusters;
Buffer<uint4> LightClustered_LightIndices;
Buffer<float4> LightClusteredSpotGroup_SpotLights;
Texture2D<float4> ShadowMapCommon_ShadowMapTexture;
SamplerState Texturing_LinearBorderSampler;
SamplerComparisonState Texturing_LinearClampCompareLessEqualSampler;
SamplerState Texturing_LinearSampler;
Texture2D<float4> MaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUT;
SamplerState DynamicSampler_Sampler;
Texture2D<float4> DynamicTexture_Texture;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_normalWS;
static float4 PS_IN_PositionWS;
static float PS_IN_DepthVS;
static float2 PS_IN_TexCoord;
static float4 PS_IN_ScreenPosition;
static bool PS_IN_IsFrontFace;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_normalWS : NORMALWS;
    float4 PS_IN_PositionWS : POSITION_WS;
    float PS_IN_DepthVS : DEPTH_VS;
    float2 PS_IN_TexCoord : TEXCOORD0;
    float4 PS_IN_ScreenPosition : SCREENPOSITION;
    bool PS_IN_IsFrontFace : SV_IsFrontFace;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float _1429;

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

void o24S251C1_IStreamInitializer_ResetStream()
{
}

void o24S251C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o24S251C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id43 = 0.0f;
}

void o24S251C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o24S251C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id7 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id8 = 0.0f.xxxx;
    _streams.matDiffuse_id9 = 0.0f.xxxx;
    _streams.matDiffuseVisible_id25 = 0.0f.xxx;
    _streams.matSpecular_id11 = 0.0f.xxx;
    _streams.matSpecularVisible_id27 = 0.0f.xxx;
    _streams.matSpecularIntensity_id12 = 1.0f;
    _streams.matGlossiness_id10 = 0.0f;
    _streams.alphaRoughness_id26 = 1.0f;
    _streams.matAmbientOcclusion_id13 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id14 = 0.0f;
    _streams.matCavity_id15 = 1.0f;
    _streams.matCavityDiffuse_id16 = 0.0f;
    _streams.matCavitySpecular_id17 = 0.0f;
    _streams.matEmissive_id18 = 0.0f.xxxx;
    _streams.matEmissiveIntensity_id19 = 0.0f;
    _streams.matScatteringStrength_id20 = 1.0f;
    _streams.matDiffuseSpecularAlphaBlend_id21 = 1.0f.xx;
    _streams.matAlphaBlendColor_id22 = 1.0f.xxx;
    _streams.matAlphaDiscard_id23 = 0.100000001490116119384765625f;
}

void o24S251C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o24S251C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id30 = 1.0f;
}

float4 o23S251C0_o11S2C0_o10S2C0_o8S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id46 * o23S251C0_o11S2C0_o10S2C0_o8S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o23S251C0_o11S2C0_o10S2C0_o8S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

float4 o23S251C0_o11S2C0_o10S2C0_o9S2C1_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return o23S251C0_o11S2C0_o10S2C0_o9S2C1_ComputeColorConstantColorLink_constantColor;
}

float4 BlendUtils_BasicBlend(float4 backColor, float4 frontColor, float3 interColor)
{
    return float4(((interColor * (frontColor.w * backColor.w)) + (frontColor.xyz * (frontColor.w * (1.0f - backColor.w)))) + (backColor.xyz * ((1.0f - frontColor.w) * backColor.w)), lerp(frontColor.w, 1.0f, backColor.w));
}

float4 o23S251C0_o11S2C0_o10S2C0_ComputeColorAdd3ds_Compute(PS_STREAMS _streams)
{
    float4 backColor = o23S251C0_o11S2C0_o10S2C0_o8S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    float4 frontColor = o23S251C0_o11S2C0_o10S2C0_o9S2C1_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    float3 interColor = frontColor.xyz + backColor.xyz;
    float4 param = backColor;
    float4 param_1 = frontColor;
    float3 param_2 = interColor;
    return BlendUtils_BasicBlend(param, param_1, param_2);
}

void o23S251C0_o11S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o23S251C0_o11S2C0_o10S2C0_ComputeColorAdd3ds_Compute(_streams);
    _streams.matDiffuse_id9 = colorBase;
    _streams.matColorBase_id8 = colorBase;
}

float4 o23S251C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return float4(o23S251C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, o23S251C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, o23S251C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, o23S251C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o23S251C0_o13S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o23S251C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0f - glossiness;
    }
    _streams.matGlossiness_id10 = glossiness;
}

float4 o23S251C0_o15S2C0_o14S2C0_ComputeColorConstantFloatLink_Material_MetalnessValue__Compute()
{
    return float4(o23S251C0_o15S2C0_o14S2C0_ComputeColorConstantFloatLink_constantFloat, o23S251C0_o15S2C0_o14S2C0_ComputeColorConstantFloatLink_constantFloat, o23S251C0_o15S2C0_o14S2C0_ComputeColorConstantFloatLink_constantFloat, o23S251C0_o15S2C0_o14S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o23S251C0_o15S2C0_MaterialSurfaceMetalness_Compute(inout PS_STREAMS _streams)
{
    float metalness = o23S251C0_o15S2C0_o14S2C0_ComputeColorConstantFloatLink_Material_MetalnessValue__Compute().x;
    _streams.matSpecular_id11 = lerp(0.0199999995529651641845703125f.xxx, _streams.matDiffuse_id9.xyz, metalness.xxx);
    float3 _2726 = lerp(_streams.matDiffuse_id9.xyz, 0.0f.xxx, metalness.xxx);
    _streams.matDiffuse_id9 = float4(_2726.x, _2726.y, _2726.z, _streams.matDiffuse_id9.w);
}

void NormalFromMesh_UpdateNormalFromTangentSpace(float3 normalInTangentSpace)
{
}

void o23S251C0_o22S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id35 = 0.0f.xxx;
    _streams.lightDirectionWS_id36 = 0.0f.xxx;
    _streams.lightColor_id37 = 0.0f.xxx;
    _streams.lightColorNdotL_id38 = 0.0f.xxx;
    _streams.envLightDiffuseColor_id39 = 0.0f.xxx;
    _streams.envLightSpecularColor_id40 = 0.0f.xxx;
    _streams.lightDirectAmbientOcclusion_id42 = 1.0f;
    _streams.NdotL_id41 = 0.0f;
}

void o23S251C0_o22S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id42 = lerp(1.0f, _streams.matAmbientOcclusion_id13, _streams.matAmbientOcclusionDirectLightingFactor_id14);
    _streams.matDiffuseVisible_id25 = ((_streams.matDiffuse_id9.xyz * lerp(1.0f, _streams.matCavity_id15, _streams.matCavityDiffuse_id16)) * _streams.matDiffuseSpecularAlphaBlend_id21.x) * _streams.matAlphaBlendColor_id22;
    _streams.matSpecularVisible_id27 = (((_streams.matSpecular_id11 * _streams.matSpecularIntensity_id12) * lerp(1.0f, _streams.matCavity_id15, _streams.matCavitySpecular_id17)) * _streams.matDiffuseSpecularAlphaBlend_id21.y) * _streams.matAlphaBlendColor_id22;
    _streams.NdotV_id28 = max(dot(_streams.normalWS_id4, _streams.viewWS_id24), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id10;
    _streams.alphaRoughness_id26 = max(roughness * roughness, 0.001000000047497451305389404296875f);
}

void o23S251C0_o22S2C0_o16S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o23S251C0_o22S2C0_o21S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o0S437C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S437C0_LightDirectionalGroup_1__GetMaxLightCount()
{
    return 1;
}

int o0S437C0_DirectLightGroupPerView_GetLightCount()
{
    return o0S437C0_DirectLightGroupPerView_LightCount;
}

void o0S437C0_LightDirectionalGroup_1__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id37 = o0S437C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id36 = -o0S437C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

float3 o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__GetShadowPositionOffset(float offsetScale, float nDotL, float3 normal)
{
    float normalOffsetScale = clamp(1.0f - nDotL, 0.0f, 1.0f);
    return normal * (((2.0f * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize.x) * offsetScale) * normalOffsetScale);
}

void o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__CalculatePCFKernelParameters(float2 position, inout float2 base_uv, out float2 st)
{
    float2 uv = position * o0S437C0_ShadowMapCommon_ShadowMapTextureSize;
    base_uv = floor(uv + 0.5f.xx);
    st = (uv + 0.5f.xx) - base_uv;
    base_uv -= 0.5f.xx;
    base_uv *= o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize;
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get3x3FilterKernel(float2 base_uv, float2 st, inout float3 kernel[4])
{
    float2 uvW0 = 3.0f.xx - (st * 2.0f);
    float2 uvW1 = 1.0f.xx + (st * 2.0f);
    float2 uv0 = ((2.0f.xx - st) / uvW0) - 1.0f.xx;
    float2 uv1 = (st / uvW1) + 1.0f.xx;
    kernel[0] = float3(base_uv + (uv0 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW0.y);
    kernel[1] = float3(base_uv + (float2(uv1.x, uv0.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW0.y);
    kernel[2] = float3(base_uv + (float2(uv0.x, uv1.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW1.y);
    kernel[3] = float3(base_uv + (uv1 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW1.y);
    return 16.0f;
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(float2 position, float positionDepth)
{
    float3 _1287 = float3(position, positionDepth);
    return ShadowMapCommon_ShadowMapTexture.SampleCmpLevelZero(Texturing_LinearClampCompareLessEqualSampler, _1287.xy, _1287.z);
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get5x5FilterKernel(float2 base_uv, float2 st, inout float3 kernel[9])
{
    float2 uvW0 = 4.0f.xx - (st * 3.0f);
    float2 uvW1 = 7.0f.xx;
    float2 uvW2 = 1.0f.xx + (st * 3.0f);
    float2 uv0 = ((3.0f.xx - (st * 2.0f)) / uvW0) - 2.0f.xx;
    float2 uv1 = (3.0f.xx + st) / uvW1;
    float2 uv2 = (st / uvW2) + 2.0f.xx;
    kernel[0] = float3(base_uv + (uv0 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW0.y);
    kernel[1] = float3(base_uv + (float2(uv1.x, uv0.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW0.y);
    kernel[2] = float3(base_uv + (float2(uv2.x, uv0.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW0.y);
    kernel[3] = float3(base_uv + (float2(uv0.x, uv1.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW1.y);
    kernel[4] = float3(base_uv + (uv1 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW1.y);
    kernel[5] = float3(base_uv + (float2(uv2.x, uv1.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW1.y);
    kernel[6] = float3(base_uv + (float2(uv0.x, uv2.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW2.y);
    kernel[7] = float3(base_uv + (float2(uv1.x, uv2.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW2.y);
    kernel[8] = float3(base_uv + (uv2 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW2.y);
    return 144.0f;
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get7x7FilterKernel(float2 base_uv, float2 st, inout float3 kernel[16])
{
    float2 uvW0 = (st * 5.0f) - 6.0f.xx;
    float2 uvW1 = (st * 11.0f) - 28.0f.xx;
    float2 uvW2 = -((st * 11.0f) + 17.0f.xx);
    float2 uvW3 = -((st * 5.0f) + 1.0f.xx);
    float2 uv0 = (((st * 4.0f) - 5.0f.xx) / uvW0) - 3.0f.xx;
    float2 uv1 = (((st * 4.0f) - 16.0f.xx) / uvW1) - 1.0f.xx;
    float2 uv2 = ((-((st * 7.0f) + 5.0f.xx)) / uvW2) + 1.0f.xx;
    float2 uv3 = ((-st) / uvW3) + 3.0f.xx;
    kernel[0] = float3(base_uv + (uv0 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW0.y);
    kernel[1] = float3(base_uv + (float2(uv1.x, uv0.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW0.y);
    kernel[2] = float3(base_uv + (float2(uv2.x, uv0.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW0.y);
    kernel[3] = float3(base_uv + (float2(uv3.x, uv0.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW0.y);
    kernel[4] = float3(base_uv + (float2(uv0.x, uv1.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW1.y);
    kernel[5] = float3(base_uv + (uv1 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW1.y);
    kernel[6] = float3(base_uv + (float2(uv2.x, uv1.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW1.y);
    kernel[7] = float3(base_uv + (float2(uv3.x, uv1.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW1.y);
    kernel[8] = float3(base_uv + (float2(uv0.x, uv2.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW2.y);
    kernel[9] = float3(base_uv + (float2(uv1.x, uv2.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW2.y);
    kernel[10] = float3(base_uv + (uv2 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW2.y);
    kernel[11] = float3(base_uv + (float2(uv3.x, uv2.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW2.y);
    kernel[12] = float3(base_uv + (float2(uv0.x, uv3.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW3.y);
    kernel[13] = float3(base_uv + (float2(uv1.x, uv3.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW3.y);
    kernel[14] = float3(base_uv + (float2(uv2.x, uv3.y) * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW3.y);
    kernel[15] = float3(base_uv + (uv3 * o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW3.y);
    return 2704.0f;
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterShadow(float2 position, float positionDepth)
{
    float shadow = 0.0f;
    float2 param = position;
    float2 param_1;
    float2 param_2;
    o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__CalculatePCFKernelParameters(param, param_1, param_2);
    float2 base_uv = param_1;
    float2 st = param_2;
    if (false)
    {
        float2 param_3 = base_uv;
        float2 param_4 = st;
        float3 param_5[4];
        float _1331 = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get3x3FilterKernel(param_3, param_4, param_5);
        float3 kernel[4] = param_5;
        float normalizationFactor = _1331;
        for (int i = 0; i < 4; i++)
        {
            float2 param_6 = kernel[i].xy;
            float param_7 = positionDepth;
            shadow += (kernel[i].z * o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(param_6, param_7));
        }
        shadow /= normalizationFactor;
    }
    else
    {
        if (true)
        {
            float2 param_8 = base_uv;
            float2 param_9 = st;
            float3 param_10[9];
            float _1361 = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get5x5FilterKernel(param_8, param_9, param_10);
            float3 kernel_1[9] = param_10;
            float normalizationFactor_1 = _1361;
            for (int i_1 = 0; i_1 < 9; i_1++)
            {
                float2 param_11 = kernel_1[i_1].xy;
                float param_12 = positionDepth;
                shadow += (kernel_1[i_1].z * o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(param_11, param_12));
            }
            shadow /= normalizationFactor_1;
        }
        else
        {
            if (false)
            {
                float2 param_13 = base_uv;
                float2 param_14 = st;
                float3 param_15[16];
                float _1391 = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get7x7FilterKernel(param_13, param_14, param_15);
                float3 kernel_2[16] = param_15;
                float normalizationFactor_2 = _1391;
                for (int i_2 = 0; i_2 < 16; i_2++)
                {
                    float2 param_16 = kernel_2[i_2].xy;
                    float param_17 = positionDepth;
                    shadow += (kernel_2[i_2].z * o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(param_16, param_17));
                }
                shadow /= normalizationFactor_2;
            }
        }
    }
    return shadow;
}

float o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(float3 shadowPositionWS, int cascadeIndex, int lightIndex)
{
    float4 shadowPosition = mul(float4(shadowPositionWS, 1.0f), o0S437C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[cascadeIndex + (lightIndex * 4)]);
    shadowPosition.z -= o0S437C0_ShadowMapReceiverBase_DepthBiases[lightIndex];
    float3 _396 = shadowPosition.xyz / shadowPosition.w.xxx;
    shadowPosition = float4(_396.x, _396.y, _396.z, shadowPosition.w);
    float2 param = shadowPosition.xy;
    float param_1 = shadowPosition.z;
    return o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterShadow(param, param_1);
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__GetFilterRadiusInPixels()
{
    if (false)
    {
        return 2.5f;
    }
    else
    {
        if (true)
        {
            return 3.5f;
        }
        else
        {
            return 4.5f;
        }
    }
}

float4 o0S437C0_Math_Project(float4 vec, float4x4 mat)
{
    float4 vecProjected = mul(vec, mat);
    float3 _603 = vecProjected.xyz / vecProjected.w.xxx;
    vecProjected = float4(_603.x, _603.y, _603.z, vecProjected.w);
    return vecProjected;
}

void o0S437C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPosition(float filterRadiusInPixels, float3 pixelPositionWS, float3 meshNormalWS, float4x4 worldToShadowCascadeUV, float4x4 inverseWorldToShadowCascadeUV, inout float3 adjustedPixelPositionWS, out float3 adjustedPixelPositionShadowSpace)
{
    float4 param = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4x4 param_1 = inverseWorldToShadowCascadeUV;
    float4 bottomLeftTexelWS = o0S437C0_Math_Project(param, param_1);
    float4 param_2 = float4(o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize * filterRadiusInPixels, 0.0f, 1.0f);
    float4x4 param_3 = inverseWorldToShadowCascadeUV;
    float4 topRightTexelWS = o0S437C0_Math_Project(param_2, param_3);
    float texelDiagonalLength = distance(topRightTexelWS.xyz, bottomLeftTexelWS.xyz);
    float3 positionOffsetWS = meshNormalWS * texelDiagonalLength;
    adjustedPixelPositionWS = pixelPositionWS - positionOffsetWS;
    float4 param_4 = float4(adjustedPixelPositionWS, 1.0f);
    float4x4 param_5 = worldToShadowCascadeUV;
    float4 shadowMapCoordinate = o0S437C0_Math_Project(param_4, param_5);
    adjustedPixelPositionShadowSpace = shadowMapCoordinate.xyz;
}

void o0S437C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPositionPerspective(float filterRadiusInPixels, float3 pixelPositionWS, float3 meshNormalWS, float4x4 worldToShadowCascadeUV, float4x4 inverseWorldToShadowCascadeUV, inout float3 adjustedPixelPositionWS, out float3 adjustedPixelPositionShadowSpace)
{
    float4 param = float4(pixelPositionWS, 1.0f);
    float4x4 param_1 = worldToShadowCascadeUV;
    float4 shadowMapCoordinate = o0S437C0_Math_Project(param, param_1);
    float4 param_2 = float4(shadowMapCoordinate.xy + (o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize * filterRadiusInPixels), shadowMapCoordinate.z, 1.0f);
    float4x4 param_3 = inverseWorldToShadowCascadeUV;
    float4 topRightTexelWS = o0S437C0_Math_Project(param_2, param_3);
    float texelDiagonalLength = distance(topRightTexelWS.xyz, pixelPositionWS);
    float3 positionOffsetWS = meshNormalWS * texelDiagonalLength;
    adjustedPixelPositionWS = pixelPositionWS - positionOffsetWS;
    float4 param_4 = float4(adjustedPixelPositionWS, 1.0f);
    float4x4 param_5 = worldToShadowCascadeUV;
    float4 adjustedShadowMapCoordinate = o0S437C0_Math_Project(param_4, param_5);
    adjustedPixelPositionShadowSpace = adjustedShadowMapCoordinate.xyz;
}

float o0S437C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(float3 shadowSpaceCoordinate, float3 pixelPositionWS, float2 depthRanges, float4x4 inverseWorldToShadowCascadeUV, bool isOrthographic)
{
    float shadowMapDepth = ShadowMapCommon_ShadowMapTexture.SampleLevel(Texturing_LinearBorderSampler, shadowSpaceCoordinate.xy, 0.0f).x;
    float thickness;
    if (isOrthographic)
    {
        thickness = abs(shadowMapDepth - shadowSpaceCoordinate.z) * depthRanges.y;
    }
    else
    {
        float4 param = float4(shadowSpaceCoordinate.xy, shadowMapDepth, 1.0f);
        float4x4 param_1 = inverseWorldToShadowCascadeUV;
        float4 shadowmapPositionWorldSpace = o0S437C0_Math_Project(param, param_1);
        thickness = distance(shadowmapPositionWorldSpace.xyz, pixelPositionWS);
    }
    return thickness;
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleAndFilter(float3 adjustedPixelPositionWS, float3 adjustedPixelPositionShadowSpace, float2 depthRanges, float4x4 inverseWorldToShadowCascadeUV, bool isOrthographic, bool isDualParaboloid)
{
    float2 uv = adjustedPixelPositionShadowSpace.xy * o0S437C0_ShadowMapCommon_ShadowMapTextureSize;
    float2 base_uv = floor(uv + 0.5f.xx);
    float2 st = (uv + 0.5f.xx) - base_uv;
    base_uv *= o0S437C0_ShadowMapCommon_ShadowMapTextureTexelSize;
    float thickness = 0.0f;
    float normalizationFactor = 1.0f;
    if (false)
    {
        float2 param = base_uv;
        float2 param_1 = st;
        float3 param_2[4];
        float _1494 = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get3x3FilterKernel(param, param_1, param_2);
        float3 kernel[4] = param_2;
        normalizationFactor = _1494;
        for (int i = 0; i < 4; i++)
        {
            float3 param_3 = float3(kernel[i].xy, adjustedPixelPositionShadowSpace.z);
            float3 param_4 = adjustedPixelPositionWS;
            float2 param_5 = depthRanges;
            float4x4 param_6 = inverseWorldToShadowCascadeUV;
            bool param_7 = isOrthographic;
            thickness += (kernel[i].z * o0S437C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(param_3, param_4, param_5, param_6, param_7));
        }
    }
    else
    {
        if (true)
        {
            float2 param_8 = base_uv;
            float2 param_9 = st;
            float3 param_10[9];
            float _1529 = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get5x5FilterKernel(param_8, param_9, param_10);
            float3 kernel_1[9] = param_10;
            normalizationFactor = _1529;
            for (int i_1 = 0; i_1 < 9; i_1++)
            {
                float3 param_11 = float3(kernel_1[i_1].xy, adjustedPixelPositionShadowSpace.z);
                float3 param_12 = adjustedPixelPositionWS;
                float2 param_13 = depthRanges;
                float4x4 param_14 = inverseWorldToShadowCascadeUV;
                bool param_15 = isOrthographic;
                thickness += (kernel_1[i_1].z * o0S437C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(param_11, param_12, param_13, param_14, param_15));
            }
        }
        else
        {
            if (false)
            {
                float2 param_16 = base_uv;
                float2 param_17 = st;
                float3 param_18[16];
                float _1564 = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__Get7x7FilterKernel(param_16, param_17, param_18);
                float3 kernel_2[16] = param_18;
                normalizationFactor = _1564;
                for (int i_2 = 0; i_2 < 16; i_2++)
                {
                    float3 param_19 = float3(kernel_2[i_2].xy, adjustedPixelPositionShadowSpace.z);
                    float3 param_20 = adjustedPixelPositionWS;
                    float2 param_21 = depthRanges;
                    float4x4 param_22 = inverseWorldToShadowCascadeUV;
                    bool param_23 = isOrthographic;
                    thickness += (kernel_2[i_2].z * o0S437C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(param_19, param_20, param_21, param_22, param_23));
                }
            }
        }
    }
    return thickness / normalizationFactor;
}

float o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterThickness(float3 pixelPositionWS, float3 meshNormalWS, float2 depthRanges, float4x4 worldToShadowCascadeUV, float4x4 inverseWorldToShadowCascadeUV, bool isOrthographic)
{
    float3 adjustedPixelPositionWS;
    float3 adjustedPixelPositionShadowSpace;
    if (isOrthographic)
    {
        float param = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__GetFilterRadiusInPixels();
        float3 param_1 = pixelPositionWS;
        float3 param_2 = meshNormalWS;
        float4x4 param_3 = worldToShadowCascadeUV;
        float4x4 param_4 = inverseWorldToShadowCascadeUV;
        float3 param_5;
        float3 param_6;
        o0S437C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPosition(param, param_1, param_2, param_3, param_4, param_5, param_6);
        adjustedPixelPositionWS = param_5;
        adjustedPixelPositionShadowSpace = param_6;
    }
    else
    {
        float param_7 = o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__GetFilterRadiusInPixels();
        float3 param_8 = pixelPositionWS;
        float3 param_9 = meshNormalWS;
        float4x4 param_10 = worldToShadowCascadeUV;
        float4x4 param_11 = inverseWorldToShadowCascadeUV;
        float3 param_12;
        float3 param_13;
        o0S437C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPositionPerspective(param_7, param_8, param_9, param_10, param_11, param_12, param_13);
        adjustedPixelPositionWS = param_12;
        adjustedPixelPositionShadowSpace = param_13;
    }
    float3 param_14 = adjustedPixelPositionWS;
    float3 param_15 = adjustedPixelPositionShadowSpace;
    float2 param_16 = depthRanges;
    float4x4 param_17 = inverseWorldToShadowCascadeUV;
    bool param_18 = isOrthographic;
    bool param_19 = false;
    return o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleAndFilter(param_14, param_15, param_16, param_17, param_18, param_19);
}

float o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(float3 pixelPositionWS, float3 meshNormalWS, int cascadeIndex, int lightIndex, bool isOrthographic)
{
    int arrayIndex = cascadeIndex + (lightIndex * 4);
    float3 param = pixelPositionWS;
    float3 param_1 = meshNormalWS;
    float2 param_2 = o0S437C0_ShadowMapReceiverBase_DepthRanges[arrayIndex];
    float4x4 param_3 = o0S437C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[arrayIndex];
    float4x4 param_4 = o0S437C0_ShadowMapReceiverBase_InverseWorldToShadowCascadeUV[arrayIndex];
    bool param_5 = isOrthographic;
    return o0S437C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterThickness(param, param_1, param_2, param_3, param_4, param_5);
}

float3 o0S437C0_ShadowMapReceiverDirectional_4_1_true_true_false_false__ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    int cascadeIndexBase = lightIndex * 4;
    int cascadeIndex = 0;
    float3 shadow;
    float tempThickness;
    for (int i = 0; i < 3; i++)
    {
        if (_streams.DepthVS_id6 > (o0S437C0_ShadowMapReceiverDirectional_CascadeDepthSplits[cascadeIndexBase + i]))
        {
            cascadeIndex = i + 1;
        }
    }
    shadow = 1.0f.xxx;
    tempThickness = 999.0f;
    float3 shadowPosition = position;
    float param = o0S437C0_ShadowMapReceiverBase_OffsetScales[lightIndex];
    float param_1 = _streams.NdotL_id41;
    float3 param_2 = _streams.normalWS_id4;
    shadowPosition += o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__GetShadowPositionOffset(param, param_1, param_2);
    float nextSplit;
    float splitSize;
    if (cascadeIndex < 4)
    {
        float3 param_3 = shadowPosition;
        int param_4 = cascadeIndex;
        int param_5 = lightIndex;
        shadow = o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(param_3, param_4, param_5).xxx;
        if (false)
        {
            float3 param_6 = _streams.PositionWS_id5.xyz;
            float3 param_7 = _streams.meshNormalWS_id3;
            int param_8 = cascadeIndex;
            int param_9 = lightIndex;
            bool param_10 = true;
            tempThickness = o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(param_6, param_7, param_8, param_9, param_10);
        }
        nextSplit = o0S437C0_ShadowMapReceiverDirectional_CascadeDepthSplits[cascadeIndexBase + cascadeIndex];
        splitSize = nextSplit;
        if (cascadeIndex > 0)
        {
            splitSize = nextSplit - (o0S437C0_ShadowMapReceiverDirectional_CascadeDepthSplits[(cascadeIndexBase + cascadeIndex) - 1]);
        }
        float splitDist = (nextSplit - _streams.DepthVS_id6) / splitSize;
        if (splitDist < 0.20000000298023223876953125f)
        {
            float lerpAmt = smoothstep(0.0f, 0.20000000298023223876953125f, splitDist);
            if (cascadeIndex == 3)
            {
                if (false)
                {
                    shadow = lerp(1.0f.xxx, shadow, lerpAmt.xxx);
                    if (false)
                    {
                        tempThickness = lerp(0.0f, tempThickness, lerpAmt);
                    }
                }
            }
            else
            {
                if (true)
                {
                    float3 param_11 = shadowPosition;
                    int param_12 = cascadeIndex + 1;
                    int param_13 = lightIndex;
                    float nextShadow = o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(param_11, param_12, param_13);
                    shadow = lerp(nextShadow.xxx, shadow, lerpAmt.xxx);
                    if (false)
                    {
                        float3 param_14 = _streams.PositionWS_id5.xyz;
                        float3 param_15 = _streams.meshNormalWS_id3;
                        int param_16 = cascadeIndex + 1;
                        int param_17 = lightIndex;
                        bool param_18 = true;
                        float nextThickness = o0S437C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(param_14, param_15, param_16, param_17, param_18);
                        tempThickness = lerp(nextThickness, tempThickness, lerpAmt);
                    }
                }
            }
        }
    }
    _streams.thicknessWS_id45 = tempThickness;
    if (false)
    {
        float3 indexable[5] = { float3(0.0f, 1.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 0.0f), 1.0f.xxx };
        return indexable[cascadeIndex] * shadow;
    }
    return shadow;
}

void o0S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S437C0_LightDirectionalGroup_1__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    float3 _159 = o0S437C0_ShadowMapReceiverDirectional_4_1_true_true_false_false__ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id44 = _159;
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id44) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id31 = normalize(_streams.viewWS_id24 + _streams.lightDirectionWS_id36);
    _streams.NdotH_id32 = clamp(dot(_streams.normalWS_id4, _streams.H_id31), 0.0f, 1.0f);
    _streams.LdotH_id33 = clamp(dot(_streams.lightDirectionWS_id36, _streams.H_id31), 0.0f, 1.0f);
    _streams.VdotH_id34 = _streams.LdotH_id33;
}

float3 o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (false)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id27);
    }
    return ((diffuseColor / 3.1415927410125732421875f.xxx) * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.x;
}

float3 o23S251C0_o22S2C0_o21S2C0_o18S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0f - lOrVDotH, 5.0f));
}

float3 o23S251C0_o22S2C0_o21S2C0_o18S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float lOrVDotH)
{
    float3 param = f0;
    float3 param_1 = 1.0f.xxx;
    float param_2 = lOrVDotH;
    return o23S251C0_o22S2C0_o21S2C0_o18S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

float3 o23S251C0_o22S2C0_o21S2C0_o18S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, float3 f0)
{
    float3 param = f0;
    float param_1 = _streams.LdotH_id33;
    return o23S251C0_o22S2C0_o21S2C0_o18S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o23S251C0_o22S2C0_o21S2C0_o19S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5f;
    return nDotX / ((nDotX * (1.0f - k)) + k);
}

float o23S251C0_o22S2C0_o21S2C0_o19S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(float alphaR, float nDotL, float nDotV)
{
    float param = alphaR;
    float param_1 = nDotL;
    float param_2 = alphaR;
    float param_3 = nDotV;
    return (o23S251C0_o22S2C0_o21S2C0_o19S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param, param_1) * o23S251C0_o22S2C0_o21S2C0_o19S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param_2, param_3)) / (nDotL * nDotV);
}

float o23S251C0_o22S2C0_o21S2C0_o19S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id26;
    float param_1 = _streams.NdotL_id41;
    float param_2 = _streams.NdotV_id28;
    return o23S251C0_o22S2C0_o21S2C0_o19S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(param, param_1, param_2);
}

float o23S251C0_o22S2C0_o21S2C0_o20S2C2_BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0f)) + 1.0f, 9.9999997473787516355514526367188e-05f);
    return alphaR2 / ((3.1415927410125732421875f * d) * d);
}

float o23S251C0_o22S2C0_o21S2C0_o20S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id26;
    float param_1 = _streams.NdotH_id32;
    return o23S251C0_o22S2C0_o21S2C0_o20S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

float3 o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id27;
    float3 param = specularColor;
    float3 fresnel = o23S251C0_o22S2C0_o21S2C0_o18S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o23S251C0_o22S2C0_o21S2C0_o19S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o23S251C0_o22S2C0_o21S2C0_o20S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    float3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / 4.0f.xxx;
    return (reflected * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.y;
}

void o1S437C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = o1S437C0_Camera_ZProjection.y / (projectedDepth - o1S437C0_Camera_ZProjection.x);
    float2 texCoord = float2(_streams.ScreenPosition_id49.x + 1.0f, 1.0f - _streams.ScreenPosition_id49.y) * 0.5f;
    int slice = int(max(log2((depth * o1S437C0_LightClustered_ClusterDepthScale) + o1S437C0_LightClustered_ClusterDepthBias), 0.0f));
    _streams.lightData_id47 = uint2(LightClustered_LightClusters.Load(int4(int4(int2(texCoord * o1S437C0_LightClustered_ClusterStride), slice, 0).xyz, int4(int2(texCoord * o1S437C0_LightClustered_ClusterStride), slice, 0).w)).xy);
    _streams.lightIndex_id48 = int(_streams.lightData_id47.x);
}

void o1S437C0_LightClusteredPointGroup_PrepareDirectLights(inout PS_STREAMS _streams)
{
    o1S437C0_LightClustered_PrepareLightData(_streams);
}

int o1S437C0_LightClusteredPointGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id47.y & 65535u);
}

int o1S437C0_LightClusteredPointGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id47.y & 65535u);
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
    float _1850 = o1S437C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _1850;
    _streams.lightPositionWS_id35 = light.PositionWS;
    _streams.lightColor_id37 = light.Color * attenuation;
    _streams.lightDirectionWS_id36 = lightVectorNorm;
}

void o1S437C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id48).x);
    _streams.lightIndex_id48++;
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
    _streams.thicknessWS_id45 = 0.0f;
    return 1.0f.xxx;
}

void o1S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S437C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    float3 _1766 = o1S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id44 = _1766;
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id44) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void o2S437C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S437C0_LightClusteredSpotGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id47.y >> uint(16));
}

int o2S437C0_LightClusteredSpotGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id47.y >> uint(16));
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
    float _2132 = o2S437C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _2132;
    _streams.lightColor_id37 = light.Color * attenuation;
    _streams.lightDirectionWS_id36 = lightVectorNorm;
}

void o2S437C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id48).x);
    _streams.lightIndex_id48++;
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
    _streams.thicknessWS_id45 = 0.0f;
    return 1.0f.xxx;
}

void o2S437C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S437C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id4, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id5.xyz;
    int param_2 = lightIndex;
    float3 _2036 = o2S437C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id44 = _2036;
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id44) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void o3S421C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id39 = 0.0f.xxx;
    _streams.envLightSpecularColor_id40 = 0.0f.xxx;
}

void o3S421C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S421C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float3 lightColor = o3S421C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id13;
    _streams.envLightDiffuseColor_id39 = lightColor;
    _streams.envLightSpecularColor_id40 = lightColor;
}

float3 o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (false)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id27);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id39;
}

float3 o23S251C0_o22S2C0_o21S2C0_o17S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(float3 specularColor, float alphaR, float nDotV)
{
    float glossiness = 1.0f - sqrt(alphaR);
    float4 environmentLightingDFG = MaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUT.SampleLevel(Texturing_LinearSampler, float2(glossiness, nDotV), 0.0f);
    return (specularColor * environmentLightingDFG.x) + environmentLightingDFG.y.xxx;
}

float3 o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id27;
    float3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id26;
    float param_2 = _streams.NdotV_id28;
    return o23S251C0_o22S2C0_o21S2C0_o17S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(param, param_1, param_2) * _streams.envLightSpecularColor_id40;
}

void o6S421C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id39 = 0.0f.xxx;
    _streams.envLightSpecularColor_id40 = 0.0f.xxx;
}

float4 o6S421C0_o4S5C0_IComputeEnvironmentColor_Compute(float3 direction)
{
    return 0.0f.xxxx;
}

float4 o6S421C0_o5S5C1_IComputeEnvironmentColor_Compute(float3 direction)
{
    return 0.0f.xxxx;
}

void o6S421C0_LightSkyboxShader_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o6S421C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float ambientAccessibility = _streams.matAmbientOcclusion_id13;
    float3 sampleDirection = mul(_streams.normalWS_id4, float3x3(float3(o6S421C0_LightSkyboxShader_SkyMatrix[0].x, o6S421C0_LightSkyboxShader_SkyMatrix[0].y, o6S421C0_LightSkyboxShader_SkyMatrix[0].z), float3(o6S421C0_LightSkyboxShader_SkyMatrix[1].x, o6S421C0_LightSkyboxShader_SkyMatrix[1].y, o6S421C0_LightSkyboxShader_SkyMatrix[1].z), float3(o6S421C0_LightSkyboxShader_SkyMatrix[2].x, o6S421C0_LightSkyboxShader_SkyMatrix[2].y, o6S421C0_LightSkyboxShader_SkyMatrix[2].z)));
    sampleDirection = float3(sampleDirection.xy, -sampleDirection.z);
    float3 param = sampleDirection;
    _streams.envLightDiffuseColor_id39 = ((o6S421C0_o4S5C0_IComputeEnvironmentColor_Compute(param).xyz * o6S421C0_LightSkyboxShader_Intensity) * ambientAccessibility) * _streams.matDiffuseSpecularAlphaBlend_id21.x;
    sampleDirection = reflect(-_streams.viewWS_id24, _streams.normalWS_id4);
    sampleDirection = mul(sampleDirection, float3x3(float3(o6S421C0_LightSkyboxShader_SkyMatrix[0].x, o6S421C0_LightSkyboxShader_SkyMatrix[0].y, o6S421C0_LightSkyboxShader_SkyMatrix[0].z), float3(o6S421C0_LightSkyboxShader_SkyMatrix[1].x, o6S421C0_LightSkyboxShader_SkyMatrix[1].y, o6S421C0_LightSkyboxShader_SkyMatrix[1].z), float3(o6S421C0_LightSkyboxShader_SkyMatrix[2].x, o6S421C0_LightSkyboxShader_SkyMatrix[2].y, o6S421C0_LightSkyboxShader_SkyMatrix[2].z)));
    sampleDirection = float3(sampleDirection.xy, -sampleDirection.z);
    float3 param_1 = sampleDirection;
    _streams.envLightSpecularColor_id40 = ((o6S421C0_o5S5C1_IComputeEnvironmentColor_Compute(param_1).xyz * o6S421C0_LightSkyboxShader_Intensity) * ambientAccessibility) * _streams.matDiffuseSpecularAlphaBlend_id21.y;
}

void o7S421C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id39 = 0.0f.xxx;
    _streams.envLightSpecularColor_id40 = 0.0f.xxx;
}

void o23S251C0_o22S2C0_o16S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o23S251C0_o22S2C0_o21S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o23S251C0_o22S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id7;
    NormalFromMesh_UpdateNormalFromTangentSpace(param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id4 = -_streams.normalWS_id4;
    }
    o23S251C0_o22S2C0_LightStream_ResetLightStream(_streams);
    o23S251C0_o22S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o23S251C0_o22S2C0_o16S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    o23S251C0_o22S2C0_o21S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    float3 directLightingContribution = 0.0f.xxx;
    o0S437C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S437C0_LightDirectionalGroup_1__GetMaxLightCount();
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
        directLightingContribution += o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
        directLightingContribution += o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
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
        directLightingContribution += o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
        directLightingContribution += o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
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
        directLightingContribution += o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
        directLightingContribution += o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    float3 environmentLightingContribution = 0.0f.xxx;
    o3S421C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    o6S421C0_LightSkyboxShader_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    o7S421C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o23S251C0_o22S2C0_o16S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o23S251C0_o22S2C0_o21S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id29 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id30 = _streams.matDiffuse_id9.w;
    o23S251C0_o22S2C0_o16S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
    o23S251C0_o22S2C0_o21S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
}

void o23S251C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o23S251C0_o11S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o23S251C0_o13S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o23S251C0_o15S2C0_MaterialSurfaceMetalness_Compute(_streams);
    o23S251C0_o22S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id24 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id5.xyz);
    _streams.shadingColor_id29 = 0.0f.xxx;
    o24S251C1_MaterialPixelShadingStream_ResetStream(_streams);
    o23S251C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id29, _streams.shadingColorAlpha_id30);
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
    PS_STREAMS _streams = { 0.0f.xxxx, false, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx, 0.0f, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f.xxxx, 0.0f, 0.0f, 0.0f.xx, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xx, uint2(0u, 0u), 0, 0.0f.xxxx };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id4 = PS_IN_normalWS;
    _streams.PositionWS_id5 = PS_IN_PositionWS;
    _streams.DepthVS_id6 = PS_IN_DepthVS;
    _streams.TexCoord_id46 = PS_IN_TexCoord;
    _streams.ScreenPosition_id49 = PS_IN_ScreenPosition;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    _streams.ScreenPosition_id49 /= _streams.ScreenPosition_id49.w.xxxx;
    NormalBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_normalWS = stage_input.PS_IN_normalWS;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_DepthVS = stage_input.PS_IN_DepthVS;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_ScreenPosition = stage_input.PS_IN_ScreenPosition;
    PS_IN_IsFrontFace = stage_input.PS_IN_IsFrontFace;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
