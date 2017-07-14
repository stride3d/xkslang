#version 450

struct PS_STREAMS
{
    float matBlend_id0;
    vec4 ShadingPosition_id1;
    vec4 ColorTarget_id2;
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
    vec2 matDiffuseSpecularAlphaBlend_id19;
    vec3 matAlphaBlendColor_id20;
    vec3 viewWS_id21;
    vec3 matDiffuseVisible_id22;
    float alphaRoughness_id23;
    vec3 matSpecularVisible_id24;
    float NdotV_id25;
    vec3 shadingColor_id26;
    float shadingColorAlpha_id27;
    vec3 lightPositionWS_id28;
    vec3 lightDirectionWS_id29;
    vec3 lightColor_id30;
    vec3 lightColorNdotL_id31;
    vec3 envLightDiffuseColor_id32;
    vec3 envLightSpecularColor_id33;
    float NdotL_id34;
    float lightDirectAmbientOcclusion_id35;
    vec2 TexCoord_id36;
    vec2 TexCoord_id37;
    float matDisplacement_id38;
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
    layout(row_major) mat4 o23S2C0_Transformation_WorldInverse;
    layout(row_major) mat4 o23S2C0_Transformation_WorldInverseTranspose;
    layout(row_major) mat4 o23S2C0_Transformation_WorldView;
    layout(row_major) mat4 o23S2C0_Transformation_WorldViewInverse;
    layout(row_major) mat4 o23S2C0_Transformation_WorldViewProjection;
    vec3 o23S2C0_Transformation_WorldScale;
    vec4 o23S2C0_Transformation_EyeMS;
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
} PerView_var;

layout(std140) uniform PerMaterial
{
    float o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat;
    vec4 o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__constantColor;
    float o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat;
    vec2 o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__scale;
    vec2 o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__offset;
    float o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__constantFloat;
} PerMaterial_var;

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

uniform sampler2D SPIRV_Cross_Combinedo3S2C0_o2S2C0_DynamicTexture_Material_DiffuseMap_PerMaterial__Textureo3S2C0_o2S2C0_DynamicSampler_Material_Sampler_i0_PerMaterial__Sampler;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_meshNormal;
layout(location = 2) in vec4 PS_IN_meshTangent;
layout(location = 3) in vec4 PS_IN_PositionWS;
layout(location = 4) in float PS_IN_matAmbientOcclusion;
layout(location = 5) in float PS_IN_matAmbientOcclusionDirectLightingFactor;
layout(location = 6) in float PS_IN_matCavity;
layout(location = 7) in float PS_IN_matCavityDiffuse;
layout(location = 8) in float PS_IN_matCavitySpecular;
layout(location = 9) in vec2 PS_IN_matDiffuseSpecularAlphaBlend;
layout(location = 10) in vec3 PS_IN_matAlphaBlendColor;
layout(location = 11) in vec2 PS_IN_TexCoord;
layout(location = 12) in vec2 PS_IN_TexCoord_1;
layout(location = 0) out float PS_OUT_matBlend;
layout(location = 1) out vec4 PS_OUT_ColorTarget;
layout(location = 2) out vec3 PS_OUT_normalWS;
layout(location = 3) out mat3 PS_OUT_tangentToWorld;
layout(location = 4) out vec3 PS_OUT_matNormal;
layout(location = 5) out vec4 PS_OUT_matColorBase;
layout(location = 6) out vec4 PS_OUT_matDiffuse;
layout(location = 7) out float PS_OUT_matGlossiness;
layout(location = 8) out vec3 PS_OUT_matSpecular;
layout(location = 9) out float PS_OUT_matSpecularIntensity;
layout(location = 10) out vec3 PS_OUT_viewWS;
layout(location = 11) out vec3 PS_OUT_matDiffuseVisible;
layout(location = 12) out float PS_OUT_alphaRoughness;
layout(location = 13) out vec3 PS_OUT_matSpecularVisible;
layout(location = 14) out float PS_OUT_NdotV;
layout(location = 15) out vec3 PS_OUT_shadingColor;
layout(location = 16) out float PS_OUT_shadingColorAlpha;
layout(location = 17) out vec3 PS_OUT_lightPositionWS;
layout(location = 18) out vec3 PS_OUT_lightDirectionWS;
layout(location = 19) out vec3 PS_OUT_lightColor;
layout(location = 20) out vec3 PS_OUT_lightColorNdotL;
layout(location = 21) out vec3 PS_OUT_envLightDiffuseColor;
layout(location = 22) out vec3 PS_OUT_envLightSpecularColor;
layout(location = 23) out float PS_OUT_NdotL;
layout(location = 24) out float PS_OUT_lightDirectAmbientOcclusion;
layout(location = 25) out float PS_OUT_matDisplacement;

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
    _streams.matBlend_id0 = 0.0;
}

void o25S246C1_MaterialDisplacementStream_ResetStream(out PS_STREAMS _streams)
{
    o25S246C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id38 = 0.0;
}

vec4 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_Combinedo3S2C0_o2S2C0_DynamicTexture_Material_DiffuseMap_PerMaterial__Textureo3S2C0_o2S2C0_DynamicSampler_Material_Sampler_i0_PerMaterial__Sampler, (_streams.TexCoord_id36 * PerMaterial_var.o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__scale) + PerMaterial_var.o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__offset);
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
    vec2 offset = _streams.TexCoord_id37 - vec2(0.5);
    float phase = length(offset);
    float derivative = cos((((phase + (PerFrame_var.Global_Time * -0.02999999932944774627685546875)) * 2.0) * 3.1400001049041748046875) * 5.0) * 0.100000001490116119384765625;
    float param = offset.y / offset.x;
    vec2 xz = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    vec2 xy = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    vec2 _419 = (((xz.yx * sign(offset.x)) * -xy.x) * 0.5) + vec2(0.5);
    vec3 normal;
    normal = vec3(_419.x, _419.y, normal.z);
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
    return vec4(PerMaterial_var.o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, PerMaterial_var.o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, PerMaterial_var.o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, PerMaterial_var.o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat);
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
    return PerMaterial_var.o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__constantColor;
}

void o18S246C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecular_id12 = o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

vec4 o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return vec4(PerMaterial_var.o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, PerMaterial_var.o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, PerMaterial_var.o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, PerMaterial_var.o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat);
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
    _streams.lightPositionWS_id28 = vec3(0.0);
    _streams.lightDirectionWS_id29 = vec3(0.0);
    _streams.lightColor_id30 = vec3(0.0);
    _streams.lightColorNdotL_id31 = vec3(0.0);
    _streams.envLightDiffuseColor_id32 = vec3(0.0);
    _streams.envLightSpecularColor_id33 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id35 = 1.0;
    _streams.NdotL_id34 = 0.0;
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
    _streams.lightDirectAmbientOcclusion_id35 = mix(1.0, _streams.matAmbientOcclusion_id14, _streams.matAmbientOcclusionDirectLightingFactor_id15);
    _streams.matDiffuseVisible_id22 = ((_streams.matDiffuse_id10.xyz * mix(1.0, _streams.matCavity_id16, _streams.matCavityDiffuse_id17)) * _streams.matDiffuseSpecularAlphaBlend_id19.x) * _streams.matAlphaBlendColor_id20;
    _streams.matSpecularVisible_id24 = (((_streams.matSpecular_id12 * _streams.matSpecularIntensity_id13) * mix(1.0, _streams.matCavity_id16, _streams.matCavitySpecular_id18)) * _streams.matDiffuseSpecularAlphaBlend_id19.y) * _streams.matAlphaBlendColor_id20;
    _streams.NdotV_id25 = max(dot(_streams.normalWS_id5, _streams.viewWS_id21), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id11;
    vec3 param = _streams.matNormal_id8;
    float roughnessAdjust = o18S246C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id23 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875);
}

void o18S246C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id8;
    o18S246C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S246C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S246C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    vec3 directLightingContribution = vec3(0.0);
    vec3 environmentLightingContribution = vec3(0.0);
    _streams.shadingColor_id26 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id27 = _streams.matDiffuse_id10.w;
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
    _streams.viewWS_id21 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id7.xyz);
    _streams.shadingColor_id26 = vec3(0.0);
    o25S246C1_MaterialDisplacementStream_ResetStream(_streams);
    o18S246C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _15 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id2 = _15;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, vec4(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, vec2(0.0), vec2(0.0), 0.0);
    _streams.ShadingPosition_id1 = PS_IN_ShadingPosition;
    _streams.meshNormal_id3 = PS_IN_meshNormal;
    _streams.meshTangent_id4 = PS_IN_meshTangent;
    _streams.PositionWS_id7 = PS_IN_PositionWS;
    _streams.matAmbientOcclusion_id14 = PS_IN_matAmbientOcclusion;
    _streams.matAmbientOcclusionDirectLightingFactor_id15 = PS_IN_matAmbientOcclusionDirectLightingFactor;
    _streams.matCavity_id16 = PS_IN_matCavity;
    _streams.matCavityDiffuse_id17 = PS_IN_matCavityDiffuse;
    _streams.matCavitySpecular_id18 = PS_IN_matCavitySpecular;
    _streams.matDiffuseSpecularAlphaBlend_id19 = PS_IN_matDiffuseSpecularAlphaBlend;
    _streams.matAlphaBlendColor_id20 = PS_IN_matAlphaBlendColor;
    _streams.TexCoord_id36 = PS_IN_TexCoord;
    _streams.TexCoord_id37 = PS_IN_TexCoord_1;
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_matBlend = _streams.matBlend_id0;
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
    PS_OUT_normalWS = _streams.normalWS_id5;
    PS_OUT_tangentToWorld = _streams.tangentToWorld_id6;
    PS_OUT_matNormal = _streams.matNormal_id8;
    PS_OUT_matColorBase = _streams.matColorBase_id9;
    PS_OUT_matDiffuse = _streams.matDiffuse_id10;
    PS_OUT_matGlossiness = _streams.matGlossiness_id11;
    PS_OUT_matSpecular = _streams.matSpecular_id12;
    PS_OUT_matSpecularIntensity = _streams.matSpecularIntensity_id13;
    PS_OUT_viewWS = _streams.viewWS_id21;
    PS_OUT_matDiffuseVisible = _streams.matDiffuseVisible_id22;
    PS_OUT_alphaRoughness = _streams.alphaRoughness_id23;
    PS_OUT_matSpecularVisible = _streams.matSpecularVisible_id24;
    PS_OUT_NdotV = _streams.NdotV_id25;
    PS_OUT_shadingColor = _streams.shadingColor_id26;
    PS_OUT_shadingColorAlpha = _streams.shadingColorAlpha_id27;
    PS_OUT_lightPositionWS = _streams.lightPositionWS_id28;
    PS_OUT_lightDirectionWS = _streams.lightDirectionWS_id29;
    PS_OUT_lightColor = _streams.lightColor_id30;
    PS_OUT_lightColorNdotL = _streams.lightColorNdotL_id31;
    PS_OUT_envLightDiffuseColor = _streams.envLightDiffuseColor_id32;
    PS_OUT_envLightSpecularColor = _streams.envLightSpecularColor_id33;
    PS_OUT_NdotL = _streams.NdotL_id34;
    PS_OUT_lightDirectAmbientOcclusion = _streams.lightDirectAmbientOcclusion_id35;
    PS_OUT_matDisplacement = _streams.matDisplacement_id38;
}

