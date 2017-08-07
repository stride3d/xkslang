#version 450

struct LightDirectional_DirectionalLightData
{
    vec3 DirectionWS;
    vec3 Color;
};

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    float matBlend_id2;
    vec3 meshNormal_id3;
    vec4 meshTangent_id4;
    vec3 normalWS_id5;
    mat3 tangentToWorld_id6;
    vec4 PositionWS_id7;
    vec3 matNormal_id8;
    vec4 matColorBase_id9;
    vec4 matDiffuse_id10;
    float matGlossiness_id11;
    vec3 matSpecular_id12;
    float matSpecularIntensity_id13;
    float matAmbientOcclusion_id14;
    float matAmbientOcclusionDirectLightingFactor_id15;
    float matCavity_id16;
    float matCavityDiffuse_id17;
    float matCavitySpecular_id18;
    vec4 matEmissive_id19;
    float matEmissiveIntensity_id20;
    vec2 matDiffuseSpecularAlphaBlend_id21;
    vec3 matAlphaBlendColor_id22;
    float matAlphaDiscard_id23;
    vec3 viewWS_id24;
    vec3 matDiffuseVisible_id25;
    float alphaRoughness_id26;
    vec3 matSpecularVisible_id27;
    float NdotV_id28;
    vec3 shadingColor_id29;
    float shadingColorAlpha_id30;
    vec3 H_id31;
    float NdotH_id32;
    float LdotH_id33;
    float VdotH_id34;
    vec3 lightPositionWS_id35;
    vec3 lightDirectionWS_id36;
    vec3 lightColor_id37;
    vec3 lightColorNdotL_id38;
    vec3 envLightDiffuseColor_id39;
    vec3 envLightSpecularColor_id40;
    float NdotL_id41;
    float lightDirectAmbientOcclusion_id42;
    vec3 shadowColor_id43;
    vec2 TexCoord_id44;
    vec2 TexCoord_id45;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 Transformation_World;
    layout(row_major) mat4 Transformation_WorldInverse;
    layout(row_major) mat4 Transformation_WorldInverseTranspose;
    layout(row_major) mat4 Transformation_WorldView;
    layout(row_major) mat4 Transformation_WorldViewInverse;
    layout(row_major) mat4 Transformation_WorldViewProjection;
    vec3 Transformation_WorldScale;
    vec4 Transformation_EyeMS;
} PerDraw_var;

layout(std140) uniform PerView
{
    layout(row_major) mat4 Transformation_View;
    layout(row_major) mat4 Transformation_ViewInverse;
    layout(row_major) mat4 Transformation_Projection;
    layout(row_major) mat4 Transformation_ProjectionInverse;
    layout(row_major) mat4 Transformation_ViewProjection;
    vec2 Transformation_ProjScreenRay;
    vec4 Transformation_Eye;
    vec4 o0S450C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S450C0_LightDirectionalGroup_Lights[8];
    int o0S450C0_DirectLightGroupPerView_LightCount;
    vec3 o1S435C0_LightSimpleAmbient_AmbientLight;
    vec4 o1S435C0_LightSimpleAmbient__padding_PerView_Lighting;
} PerView_var;

layout(std140) uniform PerMaterial
{
    float o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec4 o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
    float o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o24S34C0_o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_constantFloat;
} PerMaterial_var;

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

uniform sampler2D SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_meshNormal;
layout(location = 2) in vec4 PS_IN_meshTangent;
layout(location = 3) in vec4 PS_IN_PositionWS;
layout(location = 4) in vec2 PS_IN_TexCoord;
layout(location = 5) in vec2 PS_IN_TexCoord_1;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void NormalBase_GenerateNormal_PS()
{
}

mat3 NormalStream_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id3 = normalize(_streams.meshNormal_id3);
    vec3 tangent = normalize(_streams.meshTangent_id4.xyz);
    vec3 bitangent = cross(_streams.meshNormal_id3, tangent) * _streams.meshTangent_id4.w;
    mat3 tangentMatrix = mat3(vec3(tangent), vec3(bitangent), vec3(_streams.meshNormal_id3));
    return tangentMatrix;
}

mat3 NormalFromNormalMapping_GetTangentWorldTransform()
{
    return mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z));
}

void NormalStream_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    mat3 _63 = NormalStream_GetTangentMatrix(_streams);
    mat3 tangentMatrix = _63;
    mat3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id6 = tangentWorldTransform * tangentMatrix;
}

void NormalFromNormalMapping_GenerateNormal_PS(out PS_STREAMS _streams)
{
    NormalBase_GenerateNormal_PS();
    NormalStream_UpdateTangentToWorld(_streams);
}

void ShaderBase_PSMain()
{
}

void o25S246C1_IStreamInitializer_ResetStream()
{
}

void o25S246C1_MaterialStream_ResetStream(out PS_STREAMS _streams)
{
    o25S246C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id2 = 0.0;
}

void o25S246C1_MaterialPixelStream_ResetStream(out PS_STREAMS _streams)
{
    o25S246C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id8 = vec3(0.0, 0.0, 1.0);
    _streams.matColorBase_id9 = vec4(0.0);
    _streams.matDiffuse_id10 = vec4(0.0);
    _streams.matDiffuseVisible_id25 = vec3(0.0);
    _streams.matSpecular_id12 = vec3(0.0);
    _streams.matSpecularVisible_id27 = vec3(0.0);
    _streams.matSpecularIntensity_id13 = 1.0;
    _streams.matGlossiness_id11 = 0.0;
    _streams.alphaRoughness_id26 = 1.0;
    _streams.matAmbientOcclusion_id14 = 1.0;
    _streams.matAmbientOcclusionDirectLightingFactor_id15 = 0.0;
    _streams.matCavity_id16 = 1.0;
    _streams.matCavityDiffuse_id17 = 0.0;
    _streams.matCavitySpecular_id18 = 0.0;
    _streams.matEmissive_id19 = vec4(0.0);
    _streams.matEmissiveIntensity_id20 = 0.0;
    _streams.matDiffuseSpecularAlphaBlend_id21 = vec2(1.0);
    _streams.matAlphaBlendColor_id22 = vec3(1.0);
    _streams.matAlphaDiscard_id23 = 0.0;
}

void o25S246C1_MaterialPixelShadingStream_ResetStream(out PS_STREAMS _streams)
{
    o25S246C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id30 = 1.0;
}

vec4 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id44 * PerMaterial_var.o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o18S246C0_o3S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id10 = colorBase;
    _streams.matColorBase_id9 = colorBase;
}

vec2 o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    return vec2(x, 1.0) / vec2(sqrt(1.0 + (x * x)));
}

vec4 o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    vec2 offset = _streams.TexCoord_id45 - vec2(0.5);
    float phase = length(offset);
    float derivative = cos((((phase + (PerFrame_var.Global_Time * -0.02999999932944774627685546875)) * 2.0) * 3.1400001049041748046875) * 5.0) * 0.100000001490116119384765625;
    float param = offset.y / offset.x;
    vec2 xz = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    vec2 xy = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    vec2 _823 = (((xz.yx * sign(offset.x)) * -xy.x) * 0.5) + vec2(0.5);
    vec3 normal;
    normal = vec3(_823.x, _823.y, normal.z);
    normal.z = xy.y;
    return vec4(normal, 1.0);
}

void o18S246C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(inout PS_STREAMS _streams)
{
    vec4 normal = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0) - vec4(1.0);
    }
    if (false)
    {
        normal.z = sqrt(max(0.0, 1.0 - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id8 = normal.xyz;
}

vec4 o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return vec4(PerMaterial_var.o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o18S246C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(out PS_STREAMS _streams)
{
    float glossiness = o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0 - glossiness;
    }
    _streams.matGlossiness_id11 = glossiness;
}

vec4 o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return PerMaterial_var.o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
}

void o18S246C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecular_id12 = o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

vec4 o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return vec4(PerMaterial_var.o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o18S246C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id13 = o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void o18S246C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id5 = normalize(_streams.tangentToWorld_id6 * normalInTangentSpace);
}

void o18S246C0_o17S2C0_LightStream_ResetLightStream(out PS_STREAMS _streams)
{
    _streams.lightPositionWS_id35 = vec3(0.0);
    _streams.lightDirectionWS_id36 = vec3(0.0);
    _streams.lightColor_id37 = vec3(0.0);
    _streams.lightColorNdotL_id38 = vec3(0.0);
    _streams.envLightDiffuseColor_id39 = vec3(0.0);
    _streams.envLightSpecularColor_id40 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id42 = 1.0;
    _streams.NdotL_id41 = 0.0;
}

float o18S246C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(PS_STREAMS _streams, vec3 averageNormal)
{
    float r = length(_streams.matNormal_id8);
    float roughnessAdjust = 0.0;
    if (r < 1.0)
    {
        roughnessAdjust = (0.5 * (1.0 - (r * r))) / ((3.0 * r) - ((r * r) * r));
    }
    return roughnessAdjust;
}

void o18S246C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id42 = mix(1.0, _streams.matAmbientOcclusion_id14, _streams.matAmbientOcclusionDirectLightingFactor_id15);
    _streams.matDiffuseVisible_id25 = ((_streams.matDiffuse_id10.xyz * mix(1.0, _streams.matCavity_id16, _streams.matCavityDiffuse_id17)) * _streams.matDiffuseSpecularAlphaBlend_id21.x) * _streams.matAlphaBlendColor_id22;
    _streams.matSpecularVisible_id27 = (((_streams.matSpecular_id12 * _streams.matSpecularIntensity_id13) * mix(1.0, _streams.matCavity_id16, _streams.matCavitySpecular_id18)) * _streams.matDiffuseSpecularAlphaBlend_id21.y) * _streams.matAlphaBlendColor_id22;
    _streams.NdotV_id28 = max(dot(_streams.normalWS_id5, _streams.viewWS_id24), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id11;
    vec3 param = _streams.matNormal_id8;
    float roughnessAdjust = o18S246C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id26 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875);
}

void o0S450C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S450C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S450C0_DirectLightGroupPerView_GetLightCount()
{
    return PerView_var.o0S450C0_DirectLightGroupPerView_LightCount;
}

void o0S450C0_LightDirectionalGroup_8__PrepareDirectLightCore(out PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id37 = PerView_var.o0S450C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id36 = -PerView_var.o0S450C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

void o0S450C0_ShadowGroup_ComputeShadow(out PS_STREAMS _streams, int lightIndex)
{
    _streams.shadowColor_id43 = vec3(1.0);
}

void o0S450C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S450C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id5, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05);
    int param_1 = lightIndex;
    o0S450C0_ShadowGroup_ComputeShadow(_streams, param_1);
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id43) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void o18S246C0_o17S2C0_MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id31 = normalize(_streams.viewWS_id24 + _streams.lightDirectionWS_id36);
    _streams.NdotH_id32 = max(dot(_streams.normalWS_id5, _streams.H_id31), 9.9999997473787516355514526367188e-05);
    _streams.LdotH_id33 = max(dot(_streams.lightDirectionWS_id36, _streams.H_id31), 9.9999997473787516355514526367188e-05);
    _streams.VdotH_id34 = _streams.LdotH_id33;
}

vec3 o18S246C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id27);
    }
    return ((diffuseColor / vec3(3.1415927410125732421875)) * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.x;
}

vec3 o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, vec3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0 - lOrVDotH, 5.0));
}

vec3 o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, float lOrVDotH)
{
    vec3 param = f0;
    vec3 param_1 = vec3(1.0);
    float param_2 = lOrVDotH;
    return o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

vec3 o18S246C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, vec3 f0)
{
    vec3 param = f0;
    float param_1 = _streams.LdotH_id33;
    return o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o18S246C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5;
    return nDotX / ((nDotX * (1.0 - k)) + k);
}

float o18S246C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(float alphaR, float nDotL, float nDotV)
{
    float param = alphaR;
    float param_1 = nDotL;
    float param_2 = alphaR;
    float param_3 = nDotV;
    return (o18S246C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param, param_1) * o18S246C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param_2, param_3)) / (nDotL * nDotV);
}

float o18S246C0_o17S2C0_o16S2C0_o14S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id26;
    float param_1 = _streams.NdotL_id41;
    float param_2 = _streams.NdotV_id28;
    return o18S246C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(param, param_1, param_2);
}

float o18S246C0_o17S2C0_o16S2C0_o15S2C2_BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    return alphaR2 / (3.1415927410125732421875 * pow(max(((nDotH * nDotH) * (alphaR2 - 1.0)) + 1.0, 9.9999997473787516355514526367188e-05), 2.0));
}

float o18S246C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id26;
    float param_1 = _streams.NdotH_id32;
    return o18S246C0_o17S2C0_o16S2C0_o15S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

vec3 o18S246C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id27;
    vec3 param = specularColor;
    vec3 fresnel = o18S246C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o18S246C0_o17S2C0_o16S2C0_o14S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o18S246C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    vec3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / vec3(4.0);
    return (reflected * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.y;
}

void o1S435C0_EnvironmentLight_PrepareEnvironmentLight(out PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id39 = vec3(0.0);
    _streams.envLightSpecularColor_id40 = vec3(0.0);
}

void o1S435C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o1S435C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    vec3 lightColor = PerView_var.o1S435C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id14;
    _streams.envLightDiffuseColor_id39 = lightColor;
    _streams.envLightSpecularColor_id40 = lightColor;
}

vec3 o18S246C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id27);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id39;
}

vec3 o18S246C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(vec3 specularColor, float alphaR, float nDotV)
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

vec3 o18S246C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id27;
    vec3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id26;
    float param_2 = _streams.NdotV_id28;
    return o18S246C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(param, param_1, param_2) * _streams.envLightSpecularColor_id40;
}

void o18S246C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id8;
    o18S246C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S246C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S246C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    vec3 directLightingContribution = vec3(0.0);
    o0S450C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S450C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S450C0_DirectLightGroupPerView_GetLightCount();
    for (int i = 0; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        int param_1 = i;
        o0S450C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        o18S246C0_o17S2C0_MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o18S246C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(_streams);
        directLightingContribution += o18S246C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    vec3 environmentLightingContribution = vec3(0.0);
    o1S435C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o18S246C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o18S246C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id29 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id30 = _streams.matDiffuse_id10.w;
}

void o18S246C0_MaterialSurfaceArray_Compute(out PS_STREAMS _streams)
{
    o18S246C0_o3S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o18S246C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(_streams);
    o18S246C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o18S246C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o18S246C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o18S246C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id24 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id7.xyz);
    _streams.shadingColor_id29 = vec3(0.0);
    o25S246C1_MaterialPixelShadingStream_ResetStream(_streams);
    o18S246C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id29, _streams.shadingColorAlpha_id30);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _12 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id1 = _12;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), 0.0, vec3(0.0), vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, vec3(0.0), vec2(0.0), vec2(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.meshNormal_id3 = PS_IN_meshNormal;
    _streams.meshTangent_id4 = PS_IN_meshTangent;
    _streams.PositionWS_id7 = PS_IN_PositionWS;
    _streams.TexCoord_id44 = PS_IN_TexCoord;
    _streams.TexCoord_id45 = PS_IN_TexCoord_1;
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

