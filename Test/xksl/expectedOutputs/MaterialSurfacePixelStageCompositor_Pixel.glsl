#version 450

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
    vec4 ShadingPosition_id21;
    vec4 ColorTarget_id22;
    vec3 lightPositionWS_id23;
    vec3 lightDirectionWS_id24;
    vec3 lightColor_id25;
    vec3 lightColorNdotL_id26;
    vec3 envLightDiffuseColor_id27;
    vec3 envLightSpecularColor_id28;
    float NdotL_id29;
    float lightDirectAmbientOcclusion_id30;
    vec3 normalWS_id31;
    mat3 tangentToWorld_id32;
    float matBlend_id33;
    vec2 TexCoord_id34;
    vec2 TexCoord_id35;
    float matDisplacement_id36;
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
} PerView_var;

layout(std140) uniform PerMaterial
{
    float ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat;
    vec4 ComputeColorConstantColorLink_Material_SpecularValue__constantColor;
    float ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat;
    vec2 ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__scale;
    vec2 ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__offset;
} PerMaterial_var;

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

uniform sampler2D SPIRV_Cross_CombinedDynamicTexture_Material_DiffuseMap_PerMaterial__TextureDynamicSampler_Material_Sampler_i0_PerMaterial__Sampler;

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
layout(location = 13) out vec4 PS_OUT_ColorTarget;
layout(location = 14) out vec3 PS_OUT_lightPositionWS;
layout(location = 15) out vec3 PS_OUT_lightDirectionWS;
layout(location = 16) out vec3 PS_OUT_lightColor;
layout(location = 17) out vec3 PS_OUT_lightColorNdotL;
layout(location = 18) out vec3 PS_OUT_envLightDiffuseColor;
layout(location = 19) out vec3 PS_OUT_envLightSpecularColor;
layout(location = 20) out float PS_OUT_NdotL;
layout(location = 21) out float PS_OUT_lightDirectAmbientOcclusion;
layout(location = 22) out vec3 PS_OUT_normalWS;
layout(location = 23) out float PS_OUT_matBlend;
layout(location = 24) out float PS_OUT_matDisplacement;

void ShaderBase_PSMain()
{
}

void o19S2C1_IStreamInitializer_ResetStream()
{
}

void o19S2C1_MaterialStream_ResetStream(out PS_STREAMS _streams)
{
    o19S2C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id33 = 0.0;
}

void o19S2C1_MaterialDisplacementStream_ResetStream(out PS_STREAMS _streams)
{
    o19S2C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id36 = 0.0;
}

vec4 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_Material_DiffuseMap_PerMaterial__TextureDynamicSampler_Material_Sampler_i0_PerMaterial__Sampler, (_streams.TexCoord_id34 * PerMaterial_var.ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__scale) + PerMaterial_var.ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__offset);
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
    vec2 offset = _streams.TexCoord_id35 - vec2(0.5);
    float phase = length(offset);
    float derivative = cos((((phase + (PerFrame_var.Global_Time * -0.02999999932944774627685546875)) * 2.0) * 3.1400001049041748046875) * 5.0) * 0.100000001490116119384765625;
    float param = offset.y / offset.x;
    vec2 xz = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    vec2 xy = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    vec2 _341 = (((xz.yx * sign(offset.x)) * -xy.x) * 0.5) + vec2(0.5);
    vec3 normal;
    normal = vec3(_341.x, _341.y, normal.z);
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
    return vec4(PerMaterial_var.ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, PerMaterial_var.ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, PerMaterial_var.ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, PerMaterial_var.ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat);
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
    return PerMaterial_var.ComputeColorConstantColorLink_Material_SpecularValue__constantColor;
}

void o18S2C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecular_id4 = o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

vec4 o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return vec4(PerMaterial_var.ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, PerMaterial_var.ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, PerMaterial_var.ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, PerMaterial_var.ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat);
}

void o18S2C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id5 = o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id31 = normalize(_streams.tangentToWorld_id32 * normalInTangentSpace);
}

void o18S2C0_o17S2C0_LightStream_ResetLightStream(out PS_STREAMS _streams)
{
    _streams.lightPositionWS_id23 = vec3(0.0);
    _streams.lightDirectionWS_id24 = vec3(0.0);
    _streams.lightColor_id25 = vec3(0.0);
    _streams.lightColorNdotL_id26 = vec3(0.0);
    _streams.envLightDiffuseColor_id27 = vec3(0.0);
    _streams.envLightSpecularColor_id28 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id30 = 1.0;
    _streams.NdotL_id29 = 0.0;
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
    _streams.lightDirectAmbientOcclusion_id30 = mix(1.0, _streams.matAmbientOcclusion_id6, _streams.matAmbientOcclusionDirectLightingFactor_id7);
    _streams.matDiffuseVisible_id14 = ((_streams.matDiffuse_id2.xyz * mix(1.0, _streams.matCavity_id8, _streams.matCavityDiffuse_id9)) * _streams.matDiffuseSpecularAlphaBlend_id11.x) * _streams.matAlphaBlendColor_id12;
    _streams.matSpecularVisible_id16 = (((_streams.matSpecular_id4 * _streams.matSpecularIntensity_id5) * mix(1.0, _streams.matCavity_id8, _streams.matCavitySpecular_id10)) * _streams.matDiffuseSpecularAlphaBlend_id11.y) * _streams.matAlphaBlendColor_id12;
    _streams.NdotV_id17 = max(dot(_streams.normalWS_id31, _streams.viewWS_id13), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id3;
    vec3 param = _streams.matNormal_id0;
    float roughnessAdjust = o18S2C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id15 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875);
}

void o18S2C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id0;
    o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S2C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S2C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    vec3 directLightingContribution = vec3(0.0);
    vec3 environmentLightingContribution = vec3(0.0);
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
    PS_STREAMS _streams = PS_STREAMS(vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec4(0.0), vec3(0.0), 0.0, vec4(0.0), vec4(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), 0.0, vec2(0.0), vec2(0.0), 0.0);
    _streams.matAmbientOcclusion_id6 = PS_IN_matAmbientOcclusion;
    _streams.matAmbientOcclusionDirectLightingFactor_id7 = PS_IN_matAmbientOcclusionDirectLightingFactor;
    _streams.matCavity_id8 = PS_IN_matCavity;
    _streams.matCavityDiffuse_id9 = PS_IN_matCavityDiffuse;
    _streams.matCavitySpecular_id10 = PS_IN_matCavitySpecular;
    _streams.matDiffuseSpecularAlphaBlend_id11 = PS_IN_matDiffuseSpecularAlphaBlend;
    _streams.matAlphaBlendColor_id12 = PS_IN_matAlphaBlendColor;
    _streams.PositionWS_id18 = PS_IN_PositionWS;
    _streams.ShadingPosition_id21 = PS_IN_ShadingPosition;
    _streams.tangentToWorld_id32 = PS_IN_tangentToWorld;
    _streams.TexCoord_id34 = PS_IN_TexCoord;
    _streams.TexCoord_id35 = PS_IN_TexCoord_1;
    ShaderBase_PSMain();
    vec4 _27 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id22 = _27;
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
    PS_OUT_ColorTarget = _streams.ColorTarget_id22;
    PS_OUT_lightPositionWS = _streams.lightPositionWS_id23;
    PS_OUT_lightDirectionWS = _streams.lightDirectionWS_id24;
    PS_OUT_lightColor = _streams.lightColor_id25;
    PS_OUT_lightColorNdotL = _streams.lightColorNdotL_id26;
    PS_OUT_envLightDiffuseColor = _streams.envLightDiffuseColor_id27;
    PS_OUT_envLightSpecularColor = _streams.envLightSpecularColor_id28;
    PS_OUT_NdotL = _streams.NdotL_id29;
    PS_OUT_lightDirectAmbientOcclusion = _streams.lightDirectAmbientOcclusion_id30;
    PS_OUT_normalWS = _streams.normalWS_id31;
    PS_OUT_matBlend = _streams.matBlend_id33;
    PS_OUT_matDisplacement = _streams.matDisplacement_id36;
}

