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
    float3 meshNormal_id3;
    float3 meshNormalWS_id4;
    float4 meshTangent_id5;
    float3 normalWS_id6;
    float3x3 tangentToWorld_id7;
    float4 PositionWS_id8;
    float DepthVS_id9;
    float3 matNormal_id10;
    float4 matColorBase_id11;
    float4 matDiffuse_id12;
    float matGlossiness_id13;
    float3 matSpecular_id14;
    float matSpecularIntensity_id15;
    float matAmbientOcclusion_id16;
    float matAmbientOcclusionDirectLightingFactor_id17;
    float matCavity_id18;
    float matCavityDiffuse_id19;
    float matCavitySpecular_id20;
    float4 matEmissive_id21;
    float matEmissiveIntensity_id22;
    float matScatteringStrength_id23;
    float2 matDiffuseSpecularAlphaBlend_id24;
    float3 matAlphaBlendColor_id25;
    float matAlphaDiscard_id26;
    float3 viewWS_id27;
    float3 matDiffuseVisible_id28;
    float alphaRoughness_id29;
    float3 matSpecularVisible_id30;
    float NdotV_id31;
    float3 shadingColor_id32;
    float shadingColorAlpha_id33;
    float3 H_id34;
    float NdotH_id35;
    float LdotH_id36;
    float VdotH_id37;
    float3 lightPositionWS_id38;
    float3 lightDirectionWS_id39;
    float3 lightColor_id40;
    float3 lightColorNdotL_id41;
    float3 envLightDiffuseColor_id42;
    float3 envLightSpecularColor_id43;
    float NdotL_id44;
    float lightDirectAmbientOcclusion_id45;
    float matBlend_id46;
    float3 shadowColor_id47;
    float thicknessWS_id48;
    float2 TexCoord_id49;
    uint2 lightData_id50;
    int lightIndex_id51;
    float4 ScreenPosition_id52;
};

cbuffer PerDraw
{
    column_major float4x4 Transformation_World;
    column_major float4x4 Transformation_WorldInverse;
    column_major float4x4 Transformation_WorldInverseTranspose;
    column_major float4x4 Transformation_WorldView;
    column_major float4x4 Transformation_WorldViewInverse;
    column_major float4x4 Transformation_WorldViewProjection;
    float3 Transformation_WorldScale;
    float4 Transformation_EyeMS;
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
    float o1S433C0_Camera_NearClipPlane;
    float o1S433C0_Camera_FarClipPlane;
    float2 o1S433C0_Camera_ZProjection;
    float2 o1S433C0_Camera_ViewSize;
    float o1S433C0_Camera_AspectRatio;
    float4 o0S433C0_ShadowMapReceiverDirectional__padding_PerView_Default;
    float o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[4];
    LightDirectional_DirectionalLightData o0S433C0_LightDirectionalGroup_Lights[1];
    int o0S433C0_DirectLightGroupPerView_LightCount;
    float o1S433C0_LightClustered_ClusterDepthScale;
    float o1S433C0_LightClustered_ClusterDepthBias;
    float2 o1S433C0_LightClustered_ClusterStride;
    float3 o3S417C0_LightSimpleAmbient_AmbientLight;
    column_major float4x4 o6S417C0_LightSkyboxShader_SkyMatrix;
    float o6S417C0_LightSkyboxShader_Intensity;
    float4 o6S417C0_LightSkyboxShader__padding_PerView_Lighting;
};
cbuffer PerView_Lighting
{
    column_major float4x4 o0S433C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[4];
    column_major float4x4 o0S433C0_ShadowMapReceiverBase_InverseWorldToShadowCascadeUV[4];
    column_major float4x4 o0S433C0_ShadowMapReceiverBase_ViewMatrices[4];
    float2 o0S433C0_ShadowMapReceiverBase_DepthRanges[4];
    float o0S433C0_ShadowMapReceiverBase_DepthBiases[1];
    float o0S433C0_ShadowMapReceiverBase_OffsetScales[1];
    float2 o0S433C0_ShadowMapCommon_ShadowMapTextureSize;
    float2 o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize;
};
cbuffer PerMaterial
{
    float o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat;
    float4 o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_constantColor;
    float2 o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat;
    float2 o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat;
    float4 o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_constantColor;
    float o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat;
    float2 o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float4 o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
};
Buffer<float4> LightClusteredPointGroup_PointLights;
Texture3D<uint4> LightClustered_LightClusters;
Buffer<uint4> LightClustered_LightIndices;
Buffer<float4> LightClusteredSpotGroup_SpotLights;
Texture2D<float4> MaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUT;
SamplerState DynamicSampler_Sampler;
Texture2D<float4> DynamicTexture_Texture;
SamplerState Texturing_LinearSampler;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_meshNormal;
static float4 PS_IN_meshTangent;
static float4 PS_IN_PositionWS;
static float PS_IN_DepthVS;
static float2 PS_IN_TexCoord;
static float4 PS_IN_ScreenPosition;
static bool PS_IN_IsFrontFace;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_meshNormal : NORMAL;
    float4 PS_IN_meshTangent : TANGENT;
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

void NormalUpdate_GenerateNormal_PS()
{
}

float3x3 NormalUpdate_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id3 = normalize(_streams.meshNormal_id3);
    float3 tangent = normalize(_streams.meshTangent_id5.xyz);
    float3 bitangent = cross(_streams.meshNormal_id3, tangent) * _streams.meshTangent_id5.w;
    float3x3 tangentMatrix = float3x3(float3(tangent), float3(bitangent), float3(_streams.meshNormal_id3));
    return tangentMatrix;
}

float3x3 NormalFromNormalMapping_GetTangentWorldTransform()
{
    return float3x3(float3(Transformation_WorldInverseTranspose[0].x, Transformation_WorldInverseTranspose[0].y, Transformation_WorldInverseTranspose[0].z), float3(Transformation_WorldInverseTranspose[1].x, Transformation_WorldInverseTranspose[1].y, Transformation_WorldInverseTranspose[1].z), float3(Transformation_WorldInverseTranspose[2].x, Transformation_WorldInverseTranspose[2].y, Transformation_WorldInverseTranspose[2].z));
}

void NormalUpdate_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    float3x3 _114 = NormalUpdate_GetTangentMatrix(_streams);
    float3x3 tangentMatrix = _114;
    float3x3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id7 = mul(tangentMatrix, tangentWorldTransform);
}

void NormalFromNormalMapping_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    NormalUpdate_GenerateNormal_PS();
    NormalUpdate_UpdateTangentToWorld(_streams);
    _streams.meshNormalWS_id4 = mul(_streams.meshNormal_id3, float3x3(float3(Transformation_WorldInverseTranspose[0].x, Transformation_WorldInverseTranspose[0].y, Transformation_WorldInverseTranspose[0].z), float3(Transformation_WorldInverseTranspose[1].x, Transformation_WorldInverseTranspose[1].y, Transformation_WorldInverseTranspose[1].z), float3(Transformation_WorldInverseTranspose[2].x, Transformation_WorldInverseTranspose[2].y, Transformation_WorldInverseTranspose[2].z)));
}

void ShaderBase_PSMain()
{
}

void o43S247C1_IStreamInitializer_ResetStream()
{
}

void o43S247C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o43S247C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id46 = 0.0f;
}

void o43S247C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o43S247C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id10 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id11 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuse_id12 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuseVisible_id28 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecular_id14 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularVisible_id30 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularIntensity_id15 = 1.0f;
    _streams.matGlossiness_id13 = 0.0f;
    _streams.alphaRoughness_id29 = 1.0f;
    _streams.matAmbientOcclusion_id16 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id17 = 0.0f;
    _streams.matCavity_id18 = 1.0f;
    _streams.matCavityDiffuse_id19 = 0.0f;
    _streams.matCavitySpecular_id20 = 0.0f;
    _streams.matEmissive_id21 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matEmissiveIntensity_id22 = 0.0f;
    _streams.matScatteringStrength_id23 = 1.0f;
    _streams.matDiffuseSpecularAlphaBlend_id24 = float2(1.0f, 1.0f);
    _streams.matAlphaBlendColor_id25 = float3(1.0f, 1.0f, 1.0f);
    _streams.matAlphaDiscard_id26 = 0.100000001490116119384765625f;
}

void o43S247C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o43S247C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id33 = 1.0f;
}

float4 o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
}

void o42S247C0_o9S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id12 = colorBase;
    _streams.matColorBase_id11 = colorBase;
}

float4 o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id49 * o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o42S247C0_o11S2C0_MaterialSurfaceNormalMap_true_true__Compute(inout PS_STREAMS _streams)
{
    float4 normal = o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0f) - 1.0f.xxxx;
    }
    if (true)
    {
        normal.z = sqrt(max(0.0f, 1.0f - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id10 = normal.xyz;
}

float4 o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return float4(o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o13S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0f - glossiness;
    }
    _streams.matGlossiness_id13 = glossiness;
}

float4 o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_constantColor;
}

void o42S247C0_o15S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecular_id14 = o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

float4 o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return float4(o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o17S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id15 = o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

float4 o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i1_Material_TextureOffset_i1__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id49 * o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset).x.xxxx;
}

void o42S247C0_o19S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusion_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusion_id16 = o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i1_Material_TextureOffset_i1__Compute(_streams).x;
}

float4 o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue__Compute()
{
    return float4(o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o21S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusionDirectLightingFactor_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusionDirectLightingFactor_id17 = o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue__Compute().x;
}

float4 o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_Material_CavityValue__Compute()
{
    return float4(o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o23S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavity_id18 = o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_Material_CavityValue__Compute().x;
}

float4 o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue__Compute()
{
    return float4(o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o25S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavityDiffuse_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavityDiffuse_id19 = o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue__Compute().x;
}

float4 o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue__Compute()
{
    return float4(o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o27S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavitySpecular_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavitySpecular_id20 = o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue__Compute().x;
}

float4 o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_EmissiveMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_i2_Material_TextureOffset_i2__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id49 * o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

float4 o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_Material_EmissiveValue__Compute()
{
    return o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_constantColor;
}

float4 o42S247C0_o31S2C0_o30S2C0_ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    float4 tex1 = o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_EmissiveMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_i2_Material_TextureOffset_i2__Compute(_streams);
    float4 tex2 = o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_Material_EmissiveValue__Compute();
    float4 mix1 = tex1 * tex2;
    return mix1;
}

void o42S247C0_o31S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissive_rgba__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissive_id21 = o42S247C0_o31S2C0_o30S2C0_ComputeColorMultiply_Compute(_streams);
}

float4 o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute()
{
    return float4(o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat, o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o33S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissiveIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissiveIntensity_id22 = o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute().x;
}

void NormalUpdate_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, float3 normalInTangentSpace)
{
    _streams.normalWS_id6 = normalize(mul(normalInTangentSpace, _streams.tangentToWorld_id7));
}

void o42S247C0_o40S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id38 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectionWS_id39 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColor_id40 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColorNdotL_id41 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightDiffuseColor_id42 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id43 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectAmbientOcclusion_id45 = 1.0f;
    _streams.NdotL_id44 = 0.0f;
}

void o42S247C0_o40S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id45 = lerp(1.0f, _streams.matAmbientOcclusion_id16, _streams.matAmbientOcclusionDirectLightingFactor_id17);
    _streams.matDiffuseVisible_id28 = ((_streams.matDiffuse_id12.xyz * lerp(1.0f, _streams.matCavity_id18, _streams.matCavityDiffuse_id19)) * _streams.matDiffuseSpecularAlphaBlend_id24.x) * _streams.matAlphaBlendColor_id25;
    _streams.matSpecularVisible_id30 = (((_streams.matSpecular_id14 * _streams.matSpecularIntensity_id15) * lerp(1.0f, _streams.matCavity_id18, _streams.matCavitySpecular_id20)) * _streams.matDiffuseSpecularAlphaBlend_id24.y) * _streams.matAlphaBlendColor_id25;
    _streams.NdotV_id31 = max(dot(_streams.normalWS_id6, _streams.viewWS_id27), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id13;
    _streams.alphaRoughness_id29 = max(roughness * roughness, 0.001000000047497451305389404296875f);
}

void o42S247C0_o40S2C0_o34S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o42S247C0_o40S2C0_o39S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o0S433C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S433C0_LightDirectionalGroup_1__GetMaxLightCount()
{
    return 1;
}

int o0S433C0_DirectLightGroupPerView_GetLightCount()
{
    return o0S433C0_DirectLightGroupPerView_LightCount;
}

void o0S433C0_LightDirectionalGroup_1__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id40 = o0S433C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id39 = -o0S433C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

float3 o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__GetShadowPositionOffset(float offsetScale, float nDotL, float3 normal)
{
    float normalOffsetScale = clamp(1.0f - nDotL, 0.0f, 1.0f);
    return normal * (((2.0f * o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize.x) * offsetScale) * normalOffsetScale);
}

float o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__FilterShadow(float2 position, float positionDepth)
{
    return 0.0f;
}

float o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(float3 shadowPositionWS, int cascadeIndex, int lightIndex)
{
    float4 shadowPosition = mul(float4(shadowPositionWS, 1.0f), o0S433C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[cascadeIndex + (lightIndex * 4)]);
    shadowPosition.z -= o0S433C0_ShadowMapReceiverBase_DepthBiases[lightIndex];
    float3 _476 = shadowPosition.xyz / shadowPosition.w.xxx;
    shadowPosition = float4(_476.x, _476.y, _476.z, shadowPosition.w);
    float2 param = shadowPosition.xy;
    float param_1 = shadowPosition.z;
    return o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__FilterShadow(param, param_1);
}

float o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__FilterThickness(float3 pixelPositionWS, float3 meshNormalWS, float2 depthRanges, float4x4 worldToShadowCascadeUV, float4x4 inverseWorldToShadowCascadeUV, bool isOrthographic)
{
    return 0.0f;
}

float o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(float3 pixelPositionWS, float3 meshNormalWS, int cascadeIndex, int lightIndex, bool isOrthographic)
{
    int arrayIndex = cascadeIndex + (lightIndex * 4);
    float3 param = pixelPositionWS;
    float3 param_1 = meshNormalWS;
    float2 param_2 = o0S433C0_ShadowMapReceiverBase_DepthRanges[arrayIndex];
    float4x4 param_3 = o0S433C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[arrayIndex];
    float4x4 param_4 = o0S433C0_ShadowMapReceiverBase_InverseWorldToShadowCascadeUV[arrayIndex];
    bool param_5 = isOrthographic;
    return o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__FilterThickness(param, param_1, param_2, param_3, param_4, param_5);
}

float3 o0S433C0_ShadowMapReceiverDirectional_4_1_true_true_false_false__ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    int cascadeIndexBase = lightIndex * 4;
    int cascadeIndex = 0;
    float3 shadow;
    float tempThickness;
    for (int i = 0; i < 3; i++)
    {
        if (_streams.DepthVS_id9 > (o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[cascadeIndexBase + i]))
        {
            cascadeIndex = i + 1;
        }
    }
    shadow = float3(1.0f, 1.0f, 1.0f);
    tempThickness = 999.0f;
    float3 shadowPosition = position;
    float param = o0S433C0_ShadowMapReceiverBase_OffsetScales[lightIndex];
    float param_1 = _streams.NdotL_id44;
    float3 param_2 = _streams.normalWS_id6;
    shadowPosition += o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__GetShadowPositionOffset(param, param_1, param_2);
    float nextSplit;
    float splitSize;
    if (cascadeIndex < 4)
    {
        float3 param_3 = shadowPosition;
        int param_4 = cascadeIndex;
        int param_5 = lightIndex;
        shadow = o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(param_3, param_4, param_5).xxx;
        if (false)
        {
            float3 param_6 = _streams.PositionWS_id8.xyz;
            float3 param_7 = _streams.meshNormalWS_id4;
            int param_8 = cascadeIndex;
            int param_9 = lightIndex;
            bool param_10 = true;
            tempThickness = o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(param_6, param_7, param_8, param_9, param_10);
        }
        nextSplit = o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[cascadeIndexBase + cascadeIndex];
        splitSize = nextSplit;
        if (cascadeIndex > 0)
        {
            splitSize = nextSplit - (o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[(cascadeIndexBase + cascadeIndex) - 1]);
        }
        float splitDist = (nextSplit - _streams.DepthVS_id9) / splitSize;
        if (splitDist < 0.20000000298023223876953125f)
        {
            float lerpAmt = smoothstep(0.0f, 0.20000000298023223876953125f, splitDist);
            if (cascadeIndex == 3)
            {
                if (false)
                {
                    shadow = lerp(float3(1.0f, 1.0f, 1.0f), shadow, lerpAmt.xxx);
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
                    float nextShadow = o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(param_11, param_12, param_13);
                    shadow = lerp(nextShadow.xxx, shadow, lerpAmt.xxx);
                    if (false)
                    {
                        float3 param_14 = _streams.PositionWS_id8.xyz;
                        float3 param_15 = _streams.meshNormalWS_id4;
                        int param_16 = cascadeIndex + 1;
                        int param_17 = lightIndex;
                        bool param_18 = true;
                        float nextThickness = o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(param_14, param_15, param_16, param_17, param_18);
                        tempThickness = lerp(nextThickness, tempThickness, lerpAmt);
                    }
                }
            }
        }
    }
    _streams.thicknessWS_id48 = tempThickness;
    if (false)
    {
        float3 indexable[5] = { float3(0.0f, 1.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f) };
        return indexable[cascadeIndex] * shadow;
    }
    return shadow;
}

void o0S433C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S433C0_LightDirectionalGroup_1__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id8.xyz;
    int param_2 = lightIndex;
    float3 _239 = o0S433C0_ShadowMapReceiverDirectional_4_1_true_true_false_false__ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id47 = _239;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id47) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id34 = normalize(_streams.viewWS_id27 + _streams.lightDirectionWS_id39);
    _streams.NdotH_id35 = clamp(dot(_streams.normalWS_id6, _streams.H_id34), 0.0f, 1.0f);
    _streams.LdotH_id36 = clamp(dot(_streams.lightDirectionWS_id39, _streams.H_id34), 0.0f, 1.0f);
    _streams.VdotH_id37 = _streams.LdotH_id36;
}

float3 o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id30);
    }
    return ((diffuseColor / 3.1415927410125732421875f.xxx) * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.x;
}

float3 o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0f - lOrVDotH, 5.0f));
}

float3 o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float lOrVDotH)
{
    float3 param = f0;
    float3 param_1 = float3(1.0f, 1.0f, 1.0f);
    float param_2 = lOrVDotH;
    return o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

float3 o42S247C0_o40S2C0_o39S2C0_o36S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, float3 f0)
{
    float3 param = f0;
    float param_1 = _streams.LdotH_id36;
    return o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o42S247C0_o40S2C0_o39S2C0_o37S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5f;
    return nDotX / ((nDotX * (1.0f - k)) + k);
}

float o42S247C0_o40S2C0_o39S2C0_o37S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(float alphaR, float nDotL, float nDotV)
{
    float param = alphaR;
    float param_1 = nDotL;
    float param_2 = alphaR;
    float param_3 = nDotV;
    return (o42S247C0_o40S2C0_o39S2C0_o37S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param, param_1) * o42S247C0_o40S2C0_o39S2C0_o37S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param_2, param_3)) / (nDotL * nDotV);
}

float o42S247C0_o40S2C0_o39S2C0_o37S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id29;
    float param_1 = _streams.NdotL_id44;
    float param_2 = _streams.NdotV_id31;
    return o42S247C0_o40S2C0_o39S2C0_o37S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(param, param_1, param_2);
}

float o42S247C0_o40S2C0_o39S2C0_o38S2C2_BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0f)) + 1.0f, 9.9999997473787516355514526367188e-05f);
    return alphaR2 / ((3.1415927410125732421875f * d) * d);
}

float o42S247C0_o40S2C0_o39S2C0_o38S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id29;
    float param_1 = _streams.NdotH_id35;
    return o42S247C0_o40S2C0_o39S2C0_o38S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

float3 o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id30;
    float3 param = specularColor;
    float3 fresnel = o42S247C0_o40S2C0_o39S2C0_o36S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o42S247C0_o40S2C0_o39S2C0_o37S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o42S247C0_o40S2C0_o39S2C0_o38S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    float3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / 4.0f.xxx;
    return (reflected * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.y;
}

void o1S433C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = o1S433C0_Camera_ZProjection.y / (projectedDepth - o1S433C0_Camera_ZProjection.x);
    float2 texCoord = float2(_streams.ScreenPosition_id52.x + 1.0f, 1.0f - _streams.ScreenPosition_id52.y) * 0.5f;
    int slice = int(max(log2((depth * o1S433C0_LightClustered_ClusterDepthScale) + o1S433C0_LightClustered_ClusterDepthBias), 0.0f));
    _streams.lightData_id50 = uint2(LightClustered_LightClusters.Load(int4(int4(int2(texCoord * o1S433C0_LightClustered_ClusterStride), slice, 0).xyz, int4(int2(texCoord * o1S433C0_LightClustered_ClusterStride), slice, 0).w)).xy);
    _streams.lightIndex_id51 = int(_streams.lightData_id50.x);
}

void o1S433C0_LightClusteredPointGroup_PrepareDirectLights(inout PS_STREAMS _streams)
{
    o1S433C0_LightClustered_PrepareLightData(_streams);
}

int o1S433C0_LightClusteredPointGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id50.y & 65535u);
}

int o1S433C0_LightClusteredPointGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id50.y & 65535u);
}

float o1S433C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o1S433C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S433C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S433C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, float3 position, out float3 lightVectorNorm)
{
    float3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / lightVectorLength.xxx;
    float lightInvSquareRadius = light.InvSquareRadius;
    float3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S433C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S433C0_LightPoint_ProcessLight(inout PS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id8.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _683 = o1S433C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _683;
    _streams.lightPositionWS_id38 = light.PositionWS;
    _streams.lightColor_id40 = light.Color * attenuation;
    _streams.lightDirectionWS_id39 = lightVectorNorm;
}

void o1S433C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id51).x);
    _streams.lightIndex_id51++;
    float4 pointLight1 = LightClusteredPointGroup_PointLights.Load(realLightIndex * 2);
    float4 pointLight2 = LightClusteredPointGroup_PointLights.Load((realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S433C0_LightPoint_ProcessLight(_streams, param);
}

float3 o1S433C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id48 = 0.0f;
    return float3(1.0f, 1.0f, 1.0f);
}

void o1S433C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S433C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id8.xyz;
    int param_2 = lightIndex;
    float3 _599 = o1S433C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id47 = _599;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id47) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void o2S433C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S433C0_LightClusteredSpotGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id50.y >> uint(16));
}

int o2S433C0_LightClusteredSpotGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id50.y >> uint(16));
}

float o2S433C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o2S433C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S433C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S433C0_LightUtil_GetAngleAttenuation(float3 lightVector, float3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0f, 1.0f);
    attenuation *= attenuation;
    return attenuation;
}

float o2S433C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, float3 position, inout float3 lightVectorNorm)
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
    attenuation *= o2S433C0_LightUtil_GetDistanceAttenuation(param, param_1);
    float3 param_2 = lightVectorNorm;
    float3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S433C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S433C0_LightSpot_ProcessLight(inout PS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id8.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _966 = o2S433C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _966;
    _streams.lightColor_id40 = light.Color * attenuation;
    _streams.lightDirectionWS_id39 = lightVectorNorm;
}

void o2S433C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id51).x);
    _streams.lightIndex_id51++;
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
    o2S433C0_LightSpot_ProcessLight(_streams, param);
}

float3 o2S433C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id48 = 0.0f;
    return float3(1.0f, 1.0f, 1.0f);
}

void o2S433C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S433C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id8.xyz;
    int param_2 = lightIndex;
    float3 _870 = o2S433C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id47 = _870;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id47) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void o3S417C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id43 = float3(0.0f, 0.0f, 0.0f);
}

void o3S417C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S417C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float3 lightColor = o3S417C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id16;
    _streams.envLightDiffuseColor_id42 = lightColor;
    _streams.envLightSpecularColor_id43 = lightColor;
}

float3 o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id30);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id42;
}

float3 o42S247C0_o40S2C0_o39S2C0_o35S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(float3 specularColor, float alphaR, float nDotV)
{
    float glossiness = 1.0f - sqrt(alphaR);
    float4 environmentLightingDFG = MaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUT.SampleLevel(Texturing_LinearSampler, float2(glossiness, nDotV), 0.0f);
    return (specularColor * environmentLightingDFG.x) + environmentLightingDFG.y.xxx;
}

float3 o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id30;
    float3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id29;
    float param_2 = _streams.NdotV_id31;
    return o42S247C0_o40S2C0_o39S2C0_o35S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(param, param_1, param_2) * _streams.envLightSpecularColor_id43;
}

void o6S417C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id43 = float3(0.0f, 0.0f, 0.0f);
}

float4 o6S417C0_o4S5C0_IComputeEnvironmentColor_Compute(float3 direction)
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float4 o6S417C0_o5S5C1_IComputeEnvironmentColor_Compute(float3 direction)
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

void o6S417C0_LightSkyboxShader_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o6S417C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float ambientAccessibility = _streams.matAmbientOcclusion_id16;
    float3 sampleDirection = mul(_streams.normalWS_id6, float3x3(float3(o6S417C0_LightSkyboxShader_SkyMatrix[0].x, o6S417C0_LightSkyboxShader_SkyMatrix[0].y, o6S417C0_LightSkyboxShader_SkyMatrix[0].z), float3(o6S417C0_LightSkyboxShader_SkyMatrix[1].x, o6S417C0_LightSkyboxShader_SkyMatrix[1].y, o6S417C0_LightSkyboxShader_SkyMatrix[1].z), float3(o6S417C0_LightSkyboxShader_SkyMatrix[2].x, o6S417C0_LightSkyboxShader_SkyMatrix[2].y, o6S417C0_LightSkyboxShader_SkyMatrix[2].z)));
    sampleDirection = float3(sampleDirection.xy, -sampleDirection.z);
    float3 param = sampleDirection;
    _streams.envLightDiffuseColor_id42 = ((o6S417C0_o4S5C0_IComputeEnvironmentColor_Compute(param).xyz * o6S417C0_LightSkyboxShader_Intensity) * ambientAccessibility) * _streams.matDiffuseSpecularAlphaBlend_id24.x;
    sampleDirection = reflect(-_streams.viewWS_id27, _streams.normalWS_id6);
    sampleDirection = mul(sampleDirection, float3x3(float3(o6S417C0_LightSkyboxShader_SkyMatrix[0].x, o6S417C0_LightSkyboxShader_SkyMatrix[0].y, o6S417C0_LightSkyboxShader_SkyMatrix[0].z), float3(o6S417C0_LightSkyboxShader_SkyMatrix[1].x, o6S417C0_LightSkyboxShader_SkyMatrix[1].y, o6S417C0_LightSkyboxShader_SkyMatrix[1].z), float3(o6S417C0_LightSkyboxShader_SkyMatrix[2].x, o6S417C0_LightSkyboxShader_SkyMatrix[2].y, o6S417C0_LightSkyboxShader_SkyMatrix[2].z)));
    sampleDirection = float3(sampleDirection.xy, -sampleDirection.z);
    float3 param_1 = sampleDirection;
    _streams.envLightSpecularColor_id43 = ((o6S417C0_o5S5C1_IComputeEnvironmentColor_Compute(param_1).xyz * o6S417C0_LightSkyboxShader_Intensity) * ambientAccessibility) * _streams.matDiffuseSpecularAlphaBlend_id24.y;
}

void o7S417C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id43 = float3(0.0f, 0.0f, 0.0f);
}

void o42S247C0_o40S2C0_o34S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o42S247C0_o40S2C0_o39S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o42S247C0_o40S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id10;
    NormalUpdate_UpdateNormalFromTangentSpace(_streams, param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id6 = -_streams.normalWS_id6;
    }
    o42S247C0_o40S2C0_LightStream_ResetLightStream(_streams);
    o42S247C0_o40S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o42S247C0_o40S2C0_o34S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    o42S247C0_o40S2C0_o39S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o0S433C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S433C0_LightDirectionalGroup_1__GetMaxLightCount();
    int count = o0S433C0_DirectLightGroupPerView_GetLightCount();
    int i = 0;
    int param_1;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o0S433C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    o1S433C0_LightClusteredPointGroup_PrepareDirectLights(_streams);
    maxLightCount = o1S433C0_LightClusteredPointGroup_GetMaxLightCount(_streams);
    count = o1S433C0_LightClusteredPointGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o1S433C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    o2S433C0_DirectLightGroup_PrepareDirectLights();
    maxLightCount = o2S433C0_LightClusteredSpotGroup_GetMaxLightCount(_streams);
    count = o2S433C0_LightClusteredSpotGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o2S433C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o3S417C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    o6S417C0_LightSkyboxShader_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    o7S417C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id32 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id33 = _streams.matDiffuse_id12.w;
    o42S247C0_o40S2C0_o34S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
    o42S247C0_o40S2C0_o39S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
}

void o42S247C0_o41S2C0_MaterialSurfaceEmissiveShading_false__Compute(inout PS_STREAMS _streams)
{
    _streams.shadingColor_id32 += (_streams.matEmissive_id21.xyz * _streams.matEmissiveIntensity_id22);
    if (false)
    {
        _streams.shadingColorAlpha_id33 = _streams.matEmissive_id21.w;
    }
}

void o42S247C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o42S247C0_o9S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o42S247C0_o11S2C0_MaterialSurfaceNormalMap_true_true__Compute(_streams);
    o42S247C0_o13S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o42S247C0_o15S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o42S247C0_o17S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o42S247C0_o19S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusion_r__Compute(_streams);
    o42S247C0_o21S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusionDirectLightingFactor_r__Compute(_streams);
    o42S247C0_o23S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavity_r__Compute(_streams);
    o42S247C0_o25S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavityDiffuse_r__Compute(_streams);
    o42S247C0_o27S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavitySpecular_r__Compute(_streams);
    o42S247C0_o31S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissive_rgba__Compute(_streams);
    o42S247C0_o33S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissiveIntensity_r__Compute(_streams);
    o42S247C0_o40S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
    o42S247C0_o41S2C0_MaterialSurfaceEmissiveShading_false__Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id27 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id8.xyz);
    _streams.shadingColor_id32 = float3(0.0f, 0.0f, 0.0f);
    o43S247C1_MaterialPixelShadingStream_ResetStream(_streams);
    o42S247C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id32, _streams.shadingColorAlpha_id33);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _13 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void NormalBase_PSMain(inout PS_STREAMS _streams)
{
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), false, float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float2(0.0f, 0.0f), uint2(0u, 0u), 0, float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.meshNormal_id3 = PS_IN_meshNormal;
    _streams.meshTangent_id5 = PS_IN_meshTangent;
    _streams.PositionWS_id8 = PS_IN_PositionWS;
    _streams.DepthVS_id9 = PS_IN_DepthVS;
    _streams.TexCoord_id49 = PS_IN_TexCoord;
    _streams.ScreenPosition_id52 = PS_IN_ScreenPosition;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    _streams.ScreenPosition_id52 /= _streams.ScreenPosition_id52.w.xxxx;
    NormalBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_meshNormal = stage_input.PS_IN_meshNormal;
    PS_IN_meshTangent = stage_input.PS_IN_meshTangent;
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
