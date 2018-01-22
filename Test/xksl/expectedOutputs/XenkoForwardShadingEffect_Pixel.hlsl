struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    bool IsFrontFace_id1;
    float4 ColorTarget_id2;
    float matBlend_id3;
    float3 meshNormal_id4;
    float3 meshNormalWS_id5;
    float4 meshTangent_id6;
    float3 normalWS_id7;
    float3x3 tangentToWorld_id8;
    float4 PositionWS_id9;
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
    float3 shadowColor_id46;
    float thicknessWS_id47;
    float2 TexCoord_id48;
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
    float4 o0S439C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S439C0_LightDirectionalGroup_Lights[8];
    int o0S439C0_DirectLightGroupPerView_LightCount;
    float3 o1S423C0_LightSimpleAmbient_AmbientLight;
    float4 o1S423C0_LightSimpleAmbient__padding_PerView_Lighting;
};
cbuffer PerMaterial
{
    float o19S267C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat;
    float4 o19S267C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
    float o19S267C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat;
    float2 o19S267C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o19S267C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o25S35C0_o23S2C0_o22S2C0_o21S2C1_ComputeColorConstantFloatLink_constantFloat;
};
cbuffer PerFrame
{
    float Global_Time;
    float Global_TimeStep;
};
Texture2D<float4> MaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUT;
SamplerState DynamicSampler_Sampler;
Texture2D<float4> DynamicTexture_Texture;
SamplerState Texturing_LinearSampler;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_meshNormal;
static float4 PS_IN_meshTangent;
static float4 PS_IN_PositionWS;
static float2 PS_IN_TexCoord;
static bool PS_IN_IsFrontFace;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_meshNormal : NORMAL;
    float4 PS_IN_meshTangent : TANGENT;
    float4 PS_IN_PositionWS : POSITION_WS;
    float2 PS_IN_TexCoord : TEXCOORD0;
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
    _streams.meshNormal_id4 = normalize(_streams.meshNormal_id4);
    float3 tangent = normalize(_streams.meshTangent_id6.xyz);
    float3 bitangent = cross(_streams.meshNormal_id4, tangent) * _streams.meshTangent_id6.w;
    float3x3 tangentMatrix = float3x3(float3(tangent), float3(bitangent), float3(_streams.meshNormal_id4));
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
    _streams.tangentToWorld_id8 = mul(tangentMatrix, tangentWorldTransform);
}

void NormalFromNormalMapping_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    NormalUpdate_GenerateNormal_PS();
    NormalUpdate_UpdateTangentToWorld(_streams);
    _streams.meshNormalWS_id5 = mul(_streams.meshNormal_id4, float3x3(float3(Transformation_WorldInverseTranspose[0].x, Transformation_WorldInverseTranspose[0].y, Transformation_WorldInverseTranspose[0].z), float3(Transformation_WorldInverseTranspose[1].x, Transformation_WorldInverseTranspose[1].y, Transformation_WorldInverseTranspose[1].z), float3(Transformation_WorldInverseTranspose[2].x, Transformation_WorldInverseTranspose[2].y, Transformation_WorldInverseTranspose[2].z)));
}

void ShaderBase_PSMain()
{
}

void o26S267C1_IStreamInitializer_ResetStream()
{
}

void o26S267C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o26S267C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id3 = 0.0f;
}

void o26S267C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o26S267C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id10 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id11 = 0.0f.xxxx;
    _streams.matDiffuse_id12 = 0.0f.xxxx;
    _streams.matDiffuseVisible_id28 = 0.0f.xxx;
    _streams.matSpecular_id14 = 0.0f.xxx;
    _streams.matSpecularVisible_id30 = 0.0f.xxx;
    _streams.matSpecularIntensity_id15 = 1.0f;
    _streams.matGlossiness_id13 = 0.0f;
    _streams.alphaRoughness_id29 = 1.0f;
    _streams.matAmbientOcclusion_id16 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id17 = 0.0f;
    _streams.matCavity_id18 = 1.0f;
    _streams.matCavityDiffuse_id19 = 0.0f;
    _streams.matCavitySpecular_id20 = 0.0f;
    _streams.matEmissive_id21 = 0.0f.xxxx;
    _streams.matEmissiveIntensity_id22 = 0.0f;
    _streams.matScatteringStrength_id23 = 1.0f;
    _streams.matDiffuseSpecularAlphaBlend_id24 = 1.0f.xx;
    _streams.matAlphaBlendColor_id25 = 1.0f.xxx;
    _streams.matAlphaDiscard_id26 = 0.100000001490116119384765625f;
}

void o26S267C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o26S267C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id33 = 1.0f;
}

float4 o19S267C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id48 * o19S267C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o19S267C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o19S267C0_o3S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o19S267C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id12 = colorBase;
    _streams.matColorBase_id11 = colorBase;
}

float2 o19S267C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    return float2(x, 1.0f) / sqrt(1.0f + (x * x)).xx;
}

float4 o19S267C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    float2 offset = _streams.TexCoord_id48 - 0.5f.xx;
    float phase = length(offset);
    float derivative = cos((((phase + (Global_Time * (-0.02999999932944774627685546875f))) * 2.0f) * 3.1400001049041748046875f) * 5.0f) * 0.100000001490116119384765625f;
    float param = offset.y / offset.x;
    float2 xz = o19S267C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    float2 xy = o19S267C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    float2 _752 = (((xz.yx * sign(offset.x)) * (-xy.x)) * 0.5f) + 0.5f.xx;
    float3 normal;
    normal = float3(_752.x, _752.y, normal.z);
    normal.z = xy.y;
    return float4(normal, 1.0f);
}

void o19S267C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(inout PS_STREAMS _streams)
{
    float4 normal = o19S267C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0f) - 1.0f.xxxx;
    }
    if (false)
    {
        normal.z = sqrt(max(0.0f, 1.0f - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id10 = normal.xyz;
}

float4 o19S267C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return float4(o19S267C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o19S267C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o19S267C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o19S267C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o19S267C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o19S267C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0f - glossiness;
    }
    _streams.matGlossiness_id13 = glossiness;
}

float4 o19S267C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return o19S267C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
}

void o19S267C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecular_id14 = o19S267C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

float4 o19S267C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return float4(o19S267C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o19S267C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o19S267C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o19S267C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o19S267C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id15 = o19S267C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void NormalUpdate_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, float3 normalInTangentSpace)
{
    _streams.normalWS_id7 = normalize(mul(normalInTangentSpace, _streams.tangentToWorld_id8));
}

void o19S267C0_o18S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id38 = 0.0f.xxx;
    _streams.lightDirectionWS_id39 = 0.0f.xxx;
    _streams.lightColor_id40 = 0.0f.xxx;
    _streams.lightColorNdotL_id41 = 0.0f.xxx;
    _streams.envLightDiffuseColor_id42 = 0.0f.xxx;
    _streams.envLightSpecularColor_id43 = 0.0f.xxx;
    _streams.lightDirectAmbientOcclusion_id45 = 1.0f;
    _streams.NdotL_id44 = 0.0f;
}

void o19S267C0_o18S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id45 = lerp(1.0f, _streams.matAmbientOcclusion_id16, _streams.matAmbientOcclusionDirectLightingFactor_id17);
    _streams.matDiffuseVisible_id28 = ((_streams.matDiffuse_id12.xyz * lerp(1.0f, _streams.matCavity_id18, _streams.matCavityDiffuse_id19)) * _streams.matDiffuseSpecularAlphaBlend_id24.x) * _streams.matAlphaBlendColor_id25;
    _streams.matSpecularVisible_id30 = (((_streams.matSpecular_id14 * _streams.matSpecularIntensity_id15) * lerp(1.0f, _streams.matCavity_id18, _streams.matCavitySpecular_id20)) * _streams.matDiffuseSpecularAlphaBlend_id24.y) * _streams.matAlphaBlendColor_id25;
    _streams.NdotV_id31 = max(dot(_streams.normalWS_id7, _streams.viewWS_id27), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id13;
    _streams.alphaRoughness_id29 = max(roughness * roughness, 0.001000000047497451305389404296875f);
}

void o19S267C0_o18S2C0_o12S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o19S267C0_o18S2C0_o17S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o0S439C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S439C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S439C0_DirectLightGroupPerView_GetLightCount()
{
    return o0S439C0_DirectLightGroupPerView_LightCount;
}

void o0S439C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id40 = o0S439C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id39 = -o0S439C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

float3 o0S439C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id47 = 0.0f;
    return 1.0f.xxx;
}

void o0S439C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S439C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id7, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    float3 _237 = o0S439C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id46 = _237;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id46) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id34 = normalize(_streams.viewWS_id27 + _streams.lightDirectionWS_id39);
    _streams.NdotH_id35 = clamp(dot(_streams.normalWS_id7, _streams.H_id34), 0.0f, 1.0f);
    _streams.LdotH_id36 = clamp(dot(_streams.lightDirectionWS_id39, _streams.H_id34), 0.0f, 1.0f);
    _streams.VdotH_id37 = _streams.LdotH_id36;
}

float3 o19S267C0_o18S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id30);
    }
    return ((diffuseColor / 3.1415927410125732421875f.xxx) * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.x;
}

float3 o19S267C0_o18S2C0_o17S2C0_o14S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0f - lOrVDotH, 5.0f));
}

float3 o19S267C0_o18S2C0_o17S2C0_o14S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float lOrVDotH)
{
    float3 param = f0;
    float3 param_1 = 1.0f.xxx;
    float param_2 = lOrVDotH;
    return o19S267C0_o18S2C0_o17S2C0_o14S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

float3 o19S267C0_o18S2C0_o17S2C0_o14S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, float3 f0)
{
    float3 param = f0;
    float param_1 = _streams.LdotH_id36;
    return o19S267C0_o18S2C0_o17S2C0_o14S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o19S267C0_o18S2C0_o17S2C0_o15S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5f;
    return nDotX / ((nDotX * (1.0f - k)) + k);
}

float o19S267C0_o18S2C0_o17S2C0_o15S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(float alphaR, float nDotL, float nDotV)
{
    float param = alphaR;
    float param_1 = nDotL;
    float param_2 = alphaR;
    float param_3 = nDotV;
    return (o19S267C0_o18S2C0_o17S2C0_o15S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param, param_1) * o19S267C0_o18S2C0_o17S2C0_o15S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param_2, param_3)) / (nDotL * nDotV);
}

float o19S267C0_o18S2C0_o17S2C0_o15S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id29;
    float param_1 = _streams.NdotL_id44;
    float param_2 = _streams.NdotV_id31;
    return o19S267C0_o18S2C0_o17S2C0_o15S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(param, param_1, param_2);
}

float o19S267C0_o18S2C0_o17S2C0_o16S2C2_BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0f)) + 1.0f, 9.9999997473787516355514526367188e-05f);
    return alphaR2 / ((3.1415927410125732421875f * d) * d);
}

float o19S267C0_o18S2C0_o17S2C0_o16S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id29;
    float param_1 = _streams.NdotH_id35;
    return o19S267C0_o18S2C0_o17S2C0_o16S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

float3 o19S267C0_o18S2C0_o17S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id30;
    float3 param = specularColor;
    float3 fresnel = o19S267C0_o18S2C0_o17S2C0_o14S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o19S267C0_o18S2C0_o17S2C0_o15S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o19S267C0_o18S2C0_o17S2C0_o16S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    float3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / 4.0f.xxx;
    return (reflected * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.y;
}

void o1S423C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = 0.0f.xxx;
    _streams.envLightSpecularColor_id43 = 0.0f.xxx;
}

void o1S423C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o1S423C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float3 lightColor = o1S423C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id16;
    _streams.envLightDiffuseColor_id42 = lightColor;
    _streams.envLightSpecularColor_id43 = lightColor;
}

float3 o19S267C0_o18S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id30);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id42;
}

float3 o19S267C0_o18S2C0_o17S2C0_o13S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(float3 specularColor, float alphaR, float nDotV)
{
    float glossiness = 1.0f - sqrt(alphaR);
    float4 environmentLightingDFG = MaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUT.SampleLevel(Texturing_LinearSampler, float2(glossiness, nDotV), 0.0f);
    return (specularColor * environmentLightingDFG.x) + environmentLightingDFG.y.xxx;
}

float3 o19S267C0_o18S2C0_o17S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id30;
    float3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id29;
    float param_2 = _streams.NdotV_id31;
    return o19S267C0_o18S2C0_o17S2C0_o13S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(param, param_1, param_2) * _streams.envLightSpecularColor_id43;
}

void o19S267C0_o18S2C0_o12S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o19S267C0_o18S2C0_o17S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o19S267C0_o18S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id10;
    NormalUpdate_UpdateNormalFromTangentSpace(_streams, param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id7 = -_streams.normalWS_id7;
    }
    o19S267C0_o18S2C0_LightStream_ResetLightStream(_streams);
    o19S267C0_o18S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o19S267C0_o18S2C0_o12S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    o19S267C0_o18S2C0_o17S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    float3 directLightingContribution = 0.0f.xxx;
    o0S439C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S439C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S439C0_DirectLightGroupPerView_GetLightCount();
    for (int i = 0; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        int param_1 = i;
        o0S439C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o19S267C0_o18S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o19S267C0_o18S2C0_o17S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    float3 environmentLightingContribution = 0.0f.xxx;
    o1S423C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o19S267C0_o18S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o19S267C0_o18S2C0_o17S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id32 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id33 = _streams.matDiffuse_id12.w;
    o19S267C0_o18S2C0_o12S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
    o19S267C0_o18S2C0_o17S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
}

void o19S267C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o19S267C0_o3S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o19S267C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(_streams);
    o19S267C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o19S267C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o19S267C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o19S267C0_o18S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id27 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id9.xyz);
    _streams.shadingColor_id32 = 0.0f.xxx;
    o26S267C1_MaterialPixelShadingStream_ResetStream(_streams);
    o19S267C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id32, _streams.shadingColorAlpha_id33);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _13 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx, false, 0.0f.xxxx, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxx, float3x3(0.0f.xxx, 0.0f.xxx, 0.0f.xxx), 0.0f.xxxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f.xxxx, 0.0f, 0.0f, 0.0f.xx, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xxx, 0.0f, 0.0f, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f, 0.0f.xxx, 0.0f, 0.0f.xx };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.meshNormal_id4 = PS_IN_meshNormal;
    _streams.meshTangent_id6 = PS_IN_meshTangent;
    _streams.PositionWS_id9 = PS_IN_PositionWS;
    _streams.TexCoord_id48 = PS_IN_TexCoord;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_meshNormal = stage_input.PS_IN_meshNormal;
    PS_IN_meshTangent = stage_input.PS_IN_meshTangent;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_IsFrontFace = stage_input.PS_IN_IsFrontFace;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
