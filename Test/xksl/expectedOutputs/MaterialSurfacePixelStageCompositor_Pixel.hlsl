struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct PS_STREAMS
{
    float3 matNormal_id0;
    float4 matColorBase_id1;
    float4 matDiffuse_id2;
    float matGlossiness_id3;
    float3 matSpecular_id4;
    float matSpecularIntensity_id5;
    float matAmbientOcclusion_id6;
    float matAmbientOcclusionDirectLightingFactor_id7;
    float matCavity_id8;
    float matCavityDiffuse_id9;
    float matCavitySpecular_id10;
    float2 matDiffuseSpecularAlphaBlend_id11;
    float3 matAlphaBlendColor_id12;
    float3 viewWS_id13;
    float3 matDiffuseVisible_id14;
    float alphaRoughness_id15;
    float3 matSpecularVisible_id16;
    float NdotV_id17;
    float4 PositionWS_id18;
    float3 shadingColor_id19;
    float shadingColorAlpha_id20;
    float3 H_id21;
    float NdotH_id22;
    float LdotH_id23;
    float VdotH_id24;
    float4 ShadingPosition_id25;
    float4 ColorTarget_id26;
    float3 lightPositionWS_id27;
    float3 lightDirectionWS_id28;
    float3 lightColor_id29;
    float3 lightColorNdotL_id30;
    float3 envLightDiffuseColor_id31;
    float3 envLightSpecularColor_id32;
    float NdotL_id33;
    float lightDirectAmbientOcclusion_id34;
    float3 normalWS_id35;
    float3x3 tangentToWorld_id36;
    float matBlend_id37;
    float3 shadowColor_id38;
    float2 TexCoord_id39;
    float matDisplacement_id40;
};

cbuffer PerView
{
    float4x4 Transformation_View;
    float4x4 Transformation_ViewInverse;
    float4x4 Transformation_Projection;
    float4x4 Transformation_ProjectionInverse;
    float4x4 Transformation_ViewProjection;
    float2 Transformation_ProjScreenRay;
    float4 Transformation_Eye;
    float4 o0S297C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S297C0_LightDirectionalGroup_Lights[8];
    int o0S297C0_DirectLightGroupPerView_LightCount;
    float3 o1S282C0_LightSimpleAmbient_AmbientLight;
    float4 o1S282C0_LightSimpleAmbient__padding_PerView_Lighting;
};
cbuffer PerMaterial
{
    float o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat;
    float4 o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
    float o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat;
    float2 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
};
cbuffer PerFrame
{
    float Global_Time;
    float Global_TimeStep;
};
SamplerState DynamicSampler_Sampler;
Texture2D<float4> DynamicTexture_Texture;

static float PS_IN_matAmbientOcclusion;
static float PS_IN_matAmbientOcclusionDirectLightingFactor;
static float PS_IN_matCavity;
static float PS_IN_matCavityDiffuse;
static float PS_IN_matCavitySpecular;
static float2 PS_IN_matDiffuseSpecularAlphaBlend;
static float3 PS_IN_matAlphaBlendColor;
static float4 PS_IN_PositionWS;
static float4 PS_IN_ShadingPosition;
static float3x3 PS_IN_tangentToWorld;
static float2 PS_IN_TexCoord;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float PS_IN_matAmbientOcclusion : TEXCOORD0;
    float PS_IN_matAmbientOcclusionDirectLightingFactor : TEXCOORD1;
    float PS_IN_matCavity : TEXCOORD2;
    float PS_IN_matCavityDiffuse : TEXCOORD3;
    float PS_IN_matCavitySpecular : TEXCOORD4;
    float2 PS_IN_matDiffuseSpecularAlphaBlend : TEXCOORD5;
    float3 PS_IN_matAlphaBlendColor : TEXCOORD6;
    float4 PS_IN_PositionWS : POSITION_WS;
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_tangentToWorld_0 : TEXCOORD9;
    float3 PS_IN_tangentToWorld_1 : TEXCOORD10;
    float3 PS_IN_tangentToWorld_2 : TEXCOORD11;
    float2 PS_IN_TexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void ShaderBase_PSMain()
{
}

void o19S2C1_IStreamInitializer_ResetStream()
{
}

void o19S2C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o19S2C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id37 = 0.0f;
}

void o19S2C1_MaterialDisplacementStream_ResetStream(inout PS_STREAMS _streams)
{
    o19S2C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id40 = 0.0f;
}

float4 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id39 * o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o18S2C0_o3S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id2 = colorBase;
    _streams.matColorBase_id1 = colorBase;
}

float2 o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    float _763 = sqrt(1.0f + (x * x));
    return float2(x, 1.0f) / float2(_763, _763);
}

float4 o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    float2 offset = _streams.TexCoord_id39 - float2(0.5f, 0.5f);
    float phase = length(offset);
    float derivative = cos((((phase + (Global_Time * (-0.02999999932944774627685546875f))) * 2.0f) * 3.1400001049041748046875f) * 5.0f) * 0.100000001490116119384765625f;
    float param = offset.y / offset.x;
    float2 xz = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    float2 xy = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    float2 _743 = (((xz.yx * sign(offset.x)) * (-xy.x)) * 0.5f) + float2(0.5f, 0.5f);
    float3 normal;
    normal = float3(_743.x, _743.y, normal.z);
    normal.z = xy.y;
    return float4(normal, 1.0f);
}

void o18S2C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(inout PS_STREAMS _streams)
{
    float4 normal = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0f) - float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    if (false)
    {
        normal.z = sqrt(max(0.0f, 1.0f - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id0 = normal.xyz;
}

float4 o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return float4(o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o18S2C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0f - glossiness;
    }
    _streams.matGlossiness_id3 = glossiness;
}

float4 o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
}

void o18S2C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecular_id4 = o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

float4 o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return float4(o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o18S2C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id5 = o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, float3 normalInTangentSpace)
{
    _streams.normalWS_id35 = normalize(mul(normalInTangentSpace, _streams.tangentToWorld_id36));
}

void o18S2C0_o17S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id27 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectionWS_id28 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColor_id29 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColorNdotL_id30 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightDiffuseColor_id31 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id32 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectAmbientOcclusion_id34 = 1.0f;
    _streams.NdotL_id33 = 0.0f;
}

float o18S2C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(PS_STREAMS _streams, float3 averageNormal)
{
    float r = length(_streams.matNormal_id0);
    float roughnessAdjust = 0.0f;
    if (r < 1.0f)
    {
        roughnessAdjust = (0.5f * (1.0f - (r * r))) / ((3.0f * r) - ((r * r) * r));
    }
    return roughnessAdjust;
}

void o18S2C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id34 = lerp(1.0f, _streams.matAmbientOcclusion_id6, _streams.matAmbientOcclusionDirectLightingFactor_id7);
    _streams.matDiffuseVisible_id14 = ((_streams.matDiffuse_id2.xyz * lerp(1.0f, _streams.matCavity_id8, _streams.matCavityDiffuse_id9)) * _streams.matDiffuseSpecularAlphaBlend_id11.x) * _streams.matAlphaBlendColor_id12;
    _streams.matSpecularVisible_id16 = (((_streams.matSpecular_id4 * _streams.matSpecularIntensity_id5) * lerp(1.0f, _streams.matCavity_id8, _streams.matCavitySpecular_id10)) * _streams.matDiffuseSpecularAlphaBlend_id11.y) * _streams.matAlphaBlendColor_id12;
    _streams.NdotV_id17 = max(dot(_streams.normalWS_id35, _streams.viewWS_id13), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id3;
    float3 param = _streams.matNormal_id0;
    float roughnessAdjust = o18S2C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id15 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875f);
}

void o0S297C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S297C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S297C0_DirectLightGroupPerView_GetLightCount()
{
    return o0S297C0_DirectLightGroupPerView_LightCount;
}

void o0S297C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id29 = o0S297C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id28 = -o0S297C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

void o0S297C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.shadowColor_id38 = float3(1.0f, 1.0f, 1.0f);
}

void o0S297C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S297C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id33 = max(dot(_streams.normalWS_id35, _streams.lightDirectionWS_id28), 9.9999997473787516355514526367188e-05f);
    int param_1 = lightIndex;
    o0S297C0_ShadowGroup_ComputeShadow(_streams, param_1);
    _streams.lightColorNdotL_id30 = ((_streams.lightColor_id29 * _streams.shadowColor_id38) * _streams.NdotL_id33) * _streams.lightDirectAmbientOcclusion_id34;
}

void o18S2C0_o17S2C0_MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id21 = normalize(_streams.viewWS_id13 + _streams.lightDirectionWS_id28);
    _streams.NdotH_id22 = max(dot(_streams.normalWS_id35, _streams.H_id21), 9.9999997473787516355514526367188e-05f);
    _streams.LdotH_id23 = max(dot(_streams.lightDirectionWS_id28, _streams.H_id21), 9.9999997473787516355514526367188e-05f);
    _streams.VdotH_id24 = _streams.LdotH_id23;
}

float3 o18S2C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id14;
    if (false)
    {
        diffuseColor *= (float3(1.0f, 1.0f, 1.0f) - _streams.matSpecularVisible_id16);
    }
    return ((diffuseColor / float3(3.1415927410125732421875f, 3.1415927410125732421875f, 3.1415927410125732421875f)) * _streams.lightColorNdotL_id30) * _streams.matDiffuseSpecularAlphaBlend_id11.x;
}

float3 o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0f - lOrVDotH, 5.0f));
}

float3 o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float lOrVDotH)
{
    float3 param = f0;
    float3 param_1 = float3(1.0f, 1.0f, 1.0f);
    float param_2 = lOrVDotH;
    return o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

float3 o18S2C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, float3 f0)
{
    float3 param = f0;
    float param_1 = _streams.LdotH_id23;
    return o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o18S2C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5f;
    return nDotX / ((nDotX * (1.0f - k)) + k);
}

float o18S2C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(float alphaR, float nDotL, float nDotV)
{
    float param = alphaR;
    float param_1 = nDotL;
    float param_2 = alphaR;
    float param_3 = nDotV;
    return (o18S2C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param, param_1) * o18S2C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param_2, param_3)) / (nDotL * nDotV);
}

float o18S2C0_o17S2C0_o16S2C0_o14S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id15;
    float param_1 = _streams.NdotL_id33;
    float param_2 = _streams.NdotV_id17;
    return o18S2C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(param, param_1, param_2);
}

float o18S2C0_o17S2C0_o16S2C0_o15S2C2_BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    return alphaR2 / (3.1415927410125732421875f * pow(max(((nDotH * nDotH) * (alphaR2 - 1.0f)) + 1.0f, 9.9999997473787516355514526367188e-05f), 2.0f));
}

float o18S2C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id15;
    float param_1 = _streams.NdotH_id22;
    return o18S2C0_o17S2C0_o16S2C0_o15S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

float3 o18S2C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id16;
    float3 param = specularColor;
    float3 fresnel = o18S2C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o18S2C0_o17S2C0_o16S2C0_o14S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o18S2C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    float3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / float3(4.0f, 4.0f, 4.0f);
    return (reflected * _streams.lightColorNdotL_id30) * _streams.matDiffuseSpecularAlphaBlend_id11.y;
}

void o1S282C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id31 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id32 = float3(0.0f, 0.0f, 0.0f);
}

void o1S282C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o1S282C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float3 lightColor = o1S282C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id6;
    _streams.envLightDiffuseColor_id31 = lightColor;
    _streams.envLightSpecularColor_id32 = lightColor;
}

float3 o18S2C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id14;
    if (false)
    {
        diffuseColor *= (float3(1.0f, 1.0f, 1.0f) - _streams.matSpecularVisible_id16);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id31;
}

float3 o18S2C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(float3 specularColor, float alphaR, float nDotV)
{
    float x = 1.0f - alphaR;
    float y = nDotV;
    float b1 = -0.16879999637603759765625f;
    float b2 = 1.894999980926513671875f;
    float b3 = 0.990299999713897705078125f;
    float b4 = -4.853000164031982421875f;
    float b5 = 8.40400028228759765625f;
    float b6 = -5.068999767303466796875f;
    float bias = clamp(min((b1 * x) + ((b2 * x) * x), ((b3 + (b4 * y)) + ((b5 * y) * y)) + (((b6 * y) * y) * y)), 0.0f, 1.0f);
    float d0 = 0.604499995708465576171875f;
    float d1 = 1.69900000095367431640625f;
    float d2 = -0.522800028324127197265625f;
    float d3 = -3.6029999256134033203125f;
    float d4 = 1.4040000438690185546875f;
    float d5 = 0.19390000402927398681640625f;
    float d6 = 2.6610000133514404296875f;
    float delta = clamp((((((d0 + (d1 * x)) + (d2 * y)) + ((d3 * x) * x)) + ((d4 * x) * y)) + ((d5 * y) * y)) + (((d6 * x) * x) * x), 0.0f, 1.0f);
    float scale = delta - bias;
    bias *= clamp(50.0f * specularColor.y, 0.0f, 1.0f);
    return (specularColor * scale) + float3(bias, bias, bias);
}

float3 o18S2C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id16;
    float3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id15;
    float param_2 = _streams.NdotV_id17;
    return o18S2C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(param, param_1, param_2) * _streams.envLightSpecularColor_id32;
}

void o18S2C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id0;
    o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S2C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S2C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o0S297C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S297C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S297C0_DirectLightGroupPerView_GetLightCount();
    for (int i = 0; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        int param_1 = i;
        o0S297C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        o18S2C0_o17S2C0_MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o18S2C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
        directLightingContribution += o18S2C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o1S282C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o18S2C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o18S2C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id19 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id20 = _streams.matDiffuse_id2.w;
}

void o18S2C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o18S2C0_o3S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o18S2C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(_streams);
    o18S2C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o18S2C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o18S2C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o18S2C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id13 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id18.xyz);
    _streams.shadingColor_id19 = float3(0.0f, 0.0f, 0.0f);
    o19S2C1_MaterialDisplacementStream_ResetStream(_streams);
    o18S2C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id19, _streams.shadingColorAlpha_id20);
}

void frag_main()
{
    PS_STREAMS _streams = { float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), 0.0f, float3(0.0f, 0.0f, 0.0f), float2(0.0f, 0.0f), 0.0f };
    _streams.matAmbientOcclusion_id6 = PS_IN_matAmbientOcclusion;
    _streams.matAmbientOcclusionDirectLightingFactor_id7 = PS_IN_matAmbientOcclusionDirectLightingFactor;
    _streams.matCavity_id8 = PS_IN_matCavity;
    _streams.matCavityDiffuse_id9 = PS_IN_matCavityDiffuse;
    _streams.matCavitySpecular_id10 = PS_IN_matCavitySpecular;
    _streams.matDiffuseSpecularAlphaBlend_id11 = PS_IN_matDiffuseSpecularAlphaBlend;
    _streams.matAlphaBlendColor_id12 = PS_IN_matAlphaBlendColor;
    _streams.PositionWS_id18 = PS_IN_PositionWS;
    _streams.ShadingPosition_id25 = PS_IN_ShadingPosition;
    _streams.tangentToWorld_id36 = PS_IN_tangentToWorld;
    _streams.TexCoord_id39 = PS_IN_TexCoord;
    ShaderBase_PSMain();
    float4 _27 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id26 = _27;
    PS_OUT_ColorTarget = _streams.ColorTarget_id26;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_matAmbientOcclusion = stage_input.PS_IN_matAmbientOcclusion;
    PS_IN_matAmbientOcclusionDirectLightingFactor = stage_input.PS_IN_matAmbientOcclusionDirectLightingFactor;
    PS_IN_matCavity = stage_input.PS_IN_matCavity;
    PS_IN_matCavityDiffuse = stage_input.PS_IN_matCavityDiffuse;
    PS_IN_matCavitySpecular = stage_input.PS_IN_matCavitySpecular;
    PS_IN_matDiffuseSpecularAlphaBlend = stage_input.PS_IN_matDiffuseSpecularAlphaBlend;
    PS_IN_matAlphaBlendColor = stage_input.PS_IN_matAlphaBlendColor;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_tangentToWorld[0] = stage_input.PS_IN_tangentToWorld_0;
    PS_IN_tangentToWorld[1] = stage_input.PS_IN_tangentToWorld_1;
    PS_IN_tangentToWorld[2] = stage_input.PS_IN_tangentToWorld_2;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
