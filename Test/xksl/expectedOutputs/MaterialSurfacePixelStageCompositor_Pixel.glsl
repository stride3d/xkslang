#version 450

struct LightDirectional_DirectionalLightData
{
    vec3 DirectionWS;
    vec3 Color;
};

struct PS_STREAMS
{
    vec3 matNormal_id0;
    vec4 matColorBase_id1;
    vec4 matDiffuse_id2;
    float matGlossiness_id3;
    vec3 matSpecular_id4;
    float matSpecularIntensity_id5;
    float matAmbientOcclusion_id6;
    float matAmbientOcclusionDirectLightingFactor_id7;
    float matCavity_id8;
    float matCavityDiffuse_id9;
    float matCavitySpecular_id10;
    vec2 matDiffuseSpecularAlphaBlend_id11;
    vec3 matAlphaBlendColor_id12;
    vec3 viewWS_id13;
    vec3 matDiffuseVisible_id14;
    float alphaRoughness_id15;
    vec3 matSpecularVisible_id16;
    float NdotV_id17;
    vec4 PositionWS_id18;
    vec3 shadingColor_id19;
    float shadingColorAlpha_id20;
    vec3 H_id21;
    float NdotH_id22;
    float LdotH_id23;
    float VdotH_id24;
    vec4 ShadingPosition_id25;
    vec4 ColorTarget_id26;
    vec3 lightPositionWS_id27;
    vec3 lightDirectionWS_id28;
    vec3 lightColor_id29;
    vec3 lightColorNdotL_id30;
    vec3 envLightDiffuseColor_id31;
    vec3 envLightSpecularColor_id32;
    float NdotL_id33;
    float lightDirectAmbientOcclusion_id34;
    vec3 normalWS_id35;
    mat3 tangentToWorld_id36;
    float matBlend_id37;
    vec3 shadowColor_id38;
    vec2 TexCoord_id39;
    vec2 TexCoord_id40;
    float matDisplacement_id41;
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
    LightDirectional_DirectionalLightData o0S297C0_LightDirectionalGroup_id0_Lights[8];
    int o0S297C0_DirectLightGroupPerView_LightCount;
    vec3 o1S282C0_LightSimpleAmbient_AmbientLight;
} PerView_var;

layout(std140) uniform PerMaterial
{
    float o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_id1_constantFloat;
    vec4 o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_id1_constantColor;
    float o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_id1_constantFloat;
    vec2 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_id1_scale;
    vec2 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_id1_offset;
} PerMaterial_var;

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

uniform sampler2D SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler;

layout(location = 0) in float PS_IN_matAmbientOcclusion;
layout(location = 1) in float PS_IN_matAmbientOcclusionDirectLightingFactor;
layout(location = 2) in float PS_IN_matCavity;
layout(location = 3) in float PS_IN_matCavityDiffuse;
layout(location = 4) in float PS_IN_matCavitySpecular;
layout(location = 5) in vec2 PS_IN_matDiffuseSpecularAlphaBlend;
layout(location = 6) in vec3 PS_IN_matAlphaBlendColor;
layout(location = 7) in vec4 PS_IN_PositionWS;
layout(location = 8) in vec4 PS_IN_ShadingPosition;
layout(location = 9) in mat3 PS_IN_tangentToWorld;
layout(location = 10) in vec2 PS_IN_TexCoord;
layout(location = 11) in vec2 PS_IN_TexCoord_1;
layout(location = 0) out vec3 PS_OUT_matNormal;
layout(location = 1) out vec4 PS_OUT_matColorBase;
layout(location = 2) out vec4 PS_OUT_matDiffuse;
layout(location = 3) out float PS_OUT_matGlossiness;
layout(location = 4) out vec3 PS_OUT_matSpecular;
layout(location = 5) out float PS_OUT_matSpecularIntensity;
layout(location = 6) out vec3 PS_OUT_viewWS;
layout(location = 7) out vec3 PS_OUT_matDiffuseVisible;
layout(location = 8) out float PS_OUT_alphaRoughness;
layout(location = 9) out vec3 PS_OUT_matSpecularVisible;
layout(location = 10) out float PS_OUT_NdotV;
layout(location = 11) out vec3 PS_OUT_shadingColor;
layout(location = 12) out float PS_OUT_shadingColorAlpha;
layout(location = 13) out vec3 PS_OUT_H;
layout(location = 14) out float PS_OUT_NdotH;
layout(location = 15) out float PS_OUT_LdotH;
layout(location = 16) out float PS_OUT_VdotH;
layout(location = 17) out vec4 PS_OUT_ColorTarget;
layout(location = 18) out vec3 PS_OUT_lightPositionWS;
layout(location = 19) out vec3 PS_OUT_lightDirectionWS;
layout(location = 20) out vec3 PS_OUT_lightColor;
layout(location = 21) out vec3 PS_OUT_lightColorNdotL;
layout(location = 22) out vec3 PS_OUT_envLightDiffuseColor;
layout(location = 23) out vec3 PS_OUT_envLightSpecularColor;
layout(location = 24) out float PS_OUT_NdotL;
layout(location = 25) out float PS_OUT_lightDirectAmbientOcclusion;
layout(location = 26) out vec3 PS_OUT_normalWS;
layout(location = 27) out float PS_OUT_matBlend;
layout(location = 28) out vec3 PS_OUT_shadowColor;
layout(location = 29) out float PS_OUT_matDisplacement;

void ShaderBase_PSMain()
{
}

void o19S2C1_IStreamInitializer_ResetStream()
{
}

void o19S2C1_MaterialStream_ResetStream(out PS_STREAMS _streams)
{
    o19S2C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id37 = 0.0;
}

void o19S2C1_MaterialDisplacementStream_ResetStream(out PS_STREAMS _streams)
{
    o19S2C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id41 = 0.0;
}

vec4 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id39 * PerMaterial_var.o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_id1_scale) + PerMaterial_var.o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_id1_offset);
}

void o18S2C0_o3S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id2 = colorBase;
    _streams.matColorBase_id1 = colorBase;
}

vec2 o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    return vec2(x, 1.0) / vec2(sqrt(1.0 + (x * x)));
}

vec4 o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    vec2 offset = _streams.TexCoord_id40 - vec2(0.5);
    float phase = length(offset);
    float derivative = cos((((phase + (PerFrame_var.Global_Time * -0.02999999932944774627685546875)) * 2.0) * 3.1400001049041748046875) * 5.0) * 0.100000001490116119384765625;
    float param = offset.y / offset.x;
    vec2 xz = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    vec2 xy = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    vec2 _743 = (((xz.yx * sign(offset.x)) * -xy.x) * 0.5) + vec2(0.5);
    vec3 normal;
    normal = vec3(_743.x, _743.y, normal.z);
    normal.z = xy.y;
    return vec4(normal, 1.0);
}

void o18S2C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(inout PS_STREAMS _streams)
{
    vec4 normal = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0) - vec4(1.0);
    }
    if (false)
    {
        normal.z = sqrt(max(0.0, 1.0 - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id0 = normal.xyz;
}

vec4 o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return vec4(PerMaterial_var.o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_id1_constantFloat, PerMaterial_var.o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_id1_constantFloat, PerMaterial_var.o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_id1_constantFloat, PerMaterial_var.o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_id1_constantFloat);
}

void o18S2C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(out PS_STREAMS _streams)
{
    float glossiness = o18S2C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0 - glossiness;
    }
    _streams.matGlossiness_id3 = glossiness;
}

vec4 o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return PerMaterial_var.o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_id1_constantColor;
}

void o18S2C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecular_id4 = o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

vec4 o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return vec4(PerMaterial_var.o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_id1_constantFloat, PerMaterial_var.o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_id1_constantFloat, PerMaterial_var.o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_id1_constantFloat, PerMaterial_var.o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_id1_constantFloat);
}

void o18S2C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id5 = o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id35 = normalize(_streams.tangentToWorld_id36 * normalInTangentSpace);
}

void o18S2C0_o17S2C0_LightStream_ResetLightStream(out PS_STREAMS _streams)
{
    _streams.lightPositionWS_id27 = vec3(0.0);
    _streams.lightDirectionWS_id28 = vec3(0.0);
    _streams.lightColor_id29 = vec3(0.0);
    _streams.lightColorNdotL_id30 = vec3(0.0);
    _streams.envLightDiffuseColor_id31 = vec3(0.0);
    _streams.envLightSpecularColor_id32 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id34 = 1.0;
    _streams.NdotL_id33 = 0.0;
}

float o18S2C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(PS_STREAMS _streams, vec3 averageNormal)
{
    float r = length(_streams.matNormal_id0);
    float roughnessAdjust = 0.0;
    if (r < 1.0)
    {
        roughnessAdjust = (0.5 * (1.0 - (r * r))) / ((3.0 * r) - ((r * r) * r));
    }
    return roughnessAdjust;
}

void o18S2C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id34 = mix(1.0, _streams.matAmbientOcclusion_id6, _streams.matAmbientOcclusionDirectLightingFactor_id7);
    _streams.matDiffuseVisible_id14 = ((_streams.matDiffuse_id2.xyz * mix(1.0, _streams.matCavity_id8, _streams.matCavityDiffuse_id9)) * _streams.matDiffuseSpecularAlphaBlend_id11.x) * _streams.matAlphaBlendColor_id12;
    _streams.matSpecularVisible_id16 = (((_streams.matSpecular_id4 * _streams.matSpecularIntensity_id5) * mix(1.0, _streams.matCavity_id8, _streams.matCavitySpecular_id10)) * _streams.matDiffuseSpecularAlphaBlend_id11.y) * _streams.matAlphaBlendColor_id12;
    _streams.NdotV_id17 = max(dot(_streams.normalWS_id35, _streams.viewWS_id13), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id3;
    vec3 param = _streams.matNormal_id0;
    float roughnessAdjust = o18S2C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id15 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875);
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
    return PerView_var.o0S297C0_DirectLightGroupPerView_LightCount;
}

void o0S297C0_LightDirectionalGroup_8__PrepareDirectLightCore(out PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id29 = PerView_var.o0S297C0_LightDirectionalGroup_id0_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id28 = -PerView_var.o0S297C0_LightDirectionalGroup_id0_Lights[lightIndex].DirectionWS;
}

void o0S297C0_ShadowGroup_ComputeShadow(out PS_STREAMS _streams, int lightIndex)
{
    _streams.shadowColor_id38 = vec3(1.0);
}

void o0S297C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S297C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id33 = max(dot(_streams.normalWS_id35, _streams.lightDirectionWS_id28), 9.9999997473787516355514526367188e-05);
    int param_1 = lightIndex;
    o0S297C0_ShadowGroup_ComputeShadow(_streams, param_1);
    _streams.lightColorNdotL_id30 = ((_streams.lightColor_id29 * _streams.shadowColor_id38) * _streams.NdotL_id33) * _streams.lightDirectAmbientOcclusion_id34;
}

void o18S2C0_o17S2C0_MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id21 = normalize(_streams.viewWS_id13 + _streams.lightDirectionWS_id28);
    _streams.NdotH_id22 = max(dot(_streams.normalWS_id35, _streams.H_id21), 9.9999997473787516355514526367188e-05);
    _streams.LdotH_id23 = max(dot(_streams.lightDirectionWS_id28, _streams.H_id21), 9.9999997473787516355514526367188e-05);
    _streams.VdotH_id24 = _streams.LdotH_id23;
}

vec3 o18S2C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id14;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id16);
    }
    return ((diffuseColor / vec3(3.1415927410125732421875)) * _streams.lightColorNdotL_id30) * _streams.matDiffuseSpecularAlphaBlend_id11.x;
}

vec3 o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, vec3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0 - lOrVDotH, 5.0));
}

vec3 o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, float lOrVDotH)
{
    vec3 param = f0;
    vec3 param_1 = vec3(1.0);
    float param_2 = lOrVDotH;
    return o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

vec3 o18S2C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, vec3 f0)
{
    vec3 param = f0;
    float param_1 = _streams.LdotH_id23;
    return o18S2C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o18S2C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5;
    return nDotX / ((nDotX * (1.0 - k)) + k);
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
    return alphaR2 / (3.1415927410125732421875 * pow(max(((nDotH * nDotH) * (alphaR2 - 1.0)) + 1.0, 9.9999997473787516355514526367188e-05), 2.0));
}

float o18S2C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id15;
    float param_1 = _streams.NdotH_id22;
    return o18S2C0_o17S2C0_o16S2C0_o15S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

vec3 o18S2C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id16;
    vec3 param = specularColor;
    vec3 fresnel = o18S2C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o18S2C0_o17S2C0_o16S2C0_o14S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o18S2C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    vec3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / vec3(4.0);
    return (reflected * _streams.lightColorNdotL_id30) * _streams.matDiffuseSpecularAlphaBlend_id11.y;
}

void o1S282C0_EnvironmentLight_PrepareEnvironmentLight(out PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id31 = vec3(0.0);
    _streams.envLightSpecularColor_id32 = vec3(0.0);
}

void o1S282C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o1S282C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    vec3 lightColor = PerView_var.o1S282C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id6;
    _streams.envLightDiffuseColor_id31 = lightColor;
    _streams.envLightSpecularColor_id32 = lightColor;
}

vec3 o18S2C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id14;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id16);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id31;
}

vec3 o18S2C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(vec3 specularColor, float alphaR, float nDotV)
{
    float x = 1.0 - alphaR;
    float y = nDotV;
    float b1 = -0.16879999637603759765625;
    float b2 = 1.894999980926513671875;
    float b3 = 0.990299999713897705078125;
    float b4 = -4.853000164031982421875;
    float b5 = 8.40400028228759765625;
    float b6 = -5.068999767303466796875;
    float bias = clamp(min((b1 * x) + ((b2 * x) * x), ((b3 + (b4 * y)) + ((b5 * y) * y)) + (((b6 * y) * y) * y)), 0.0, 1.0);
    float d0 = 0.604499995708465576171875;
    float d1 = 1.69900000095367431640625;
    float d2 = -0.522800028324127197265625;
    float d3 = -3.6029999256134033203125;
    float d4 = 1.4040000438690185546875;
    float d5 = 0.19390000402927398681640625;
    float d6 = 2.6610000133514404296875;
    float delta = clamp((((((d0 + (d1 * x)) + (d2 * y)) + ((d3 * x) * x)) + ((d4 * x) * y)) + ((d5 * y) * y)) + (((d6 * x) * x) * x), 0.0, 1.0);
    float scale = delta - bias;
    bias *= clamp(50.0 * specularColor.y, 0.0, 1.0);
    return (specularColor * scale) + vec3(bias);
}

vec3 o18S2C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id16;
    vec3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id15;
    float param_2 = _streams.NdotV_id17;
    return o18S2C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(param, param_1, param_2) * _streams.envLightSpecularColor_id32;
}

void o18S2C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id0;
    o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S2C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S2C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    vec3 directLightingContribution = vec3(0.0);
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
    vec3 environmentLightingContribution = vec3(0.0);
    o1S282C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o18S2C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o18S2C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id19 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id20 = _streams.matDiffuse_id2.w;
}

void o18S2C0_MaterialSurfaceArray_Compute(out PS_STREAMS _streams)
{
    o18S2C0_o3S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o18S2C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(_streams);
    o18S2C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o18S2C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o18S2C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o18S2C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id13 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id18.xyz);
    _streams.shadingColor_id19 = vec3(0.0);
    o19S2C1_MaterialDisplacementStream_ResetStream(_streams);
    o18S2C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id19, _streams.shadingColorAlpha_id20);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec4(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, vec4(0.0), vec4(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), 0.0, vec3(0.0), vec2(0.0), vec2(0.0), 0.0);
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
    _streams.TexCoord_id40 = PS_IN_TexCoord_1;
    ShaderBase_PSMain();
    vec4 _27 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id26 = _27;
    PS_OUT_matNormal = _streams.matNormal_id0;
    PS_OUT_matColorBase = _streams.matColorBase_id1;
    PS_OUT_matDiffuse = _streams.matDiffuse_id2;
    PS_OUT_matGlossiness = _streams.matGlossiness_id3;
    PS_OUT_matSpecular = _streams.matSpecular_id4;
    PS_OUT_matSpecularIntensity = _streams.matSpecularIntensity_id5;
    PS_OUT_viewWS = _streams.viewWS_id13;
    PS_OUT_matDiffuseVisible = _streams.matDiffuseVisible_id14;
    PS_OUT_alphaRoughness = _streams.alphaRoughness_id15;
    PS_OUT_matSpecularVisible = _streams.matSpecularVisible_id16;
    PS_OUT_NdotV = _streams.NdotV_id17;
    PS_OUT_shadingColor = _streams.shadingColor_id19;
    PS_OUT_shadingColorAlpha = _streams.shadingColorAlpha_id20;
    PS_OUT_H = _streams.H_id21;
    PS_OUT_NdotH = _streams.NdotH_id22;
    PS_OUT_LdotH = _streams.LdotH_id23;
    PS_OUT_VdotH = _streams.VdotH_id24;
    PS_OUT_ColorTarget = _streams.ColorTarget_id26;
    PS_OUT_lightPositionWS = _streams.lightPositionWS_id27;
    PS_OUT_lightDirectionWS = _streams.lightDirectionWS_id28;
    PS_OUT_lightColor = _streams.lightColor_id29;
    PS_OUT_lightColorNdotL = _streams.lightColorNdotL_id30;
    PS_OUT_envLightDiffuseColor = _streams.envLightDiffuseColor_id31;
    PS_OUT_envLightSpecularColor = _streams.envLightSpecularColor_id32;
    PS_OUT_NdotL = _streams.NdotL_id33;
    PS_OUT_lightDirectAmbientOcclusion = _streams.lightDirectAmbientOcclusion_id34;
    PS_OUT_normalWS = _streams.normalWS_id35;
    PS_OUT_matBlend = _streams.matBlend_id37;
    PS_OUT_shadowColor = _streams.shadowColor_id38;
    PS_OUT_matDisplacement = _streams.matDisplacement_id41;
}

