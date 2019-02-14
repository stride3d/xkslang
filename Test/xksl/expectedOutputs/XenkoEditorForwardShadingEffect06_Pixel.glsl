#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct LightDirectional_DirectionalLightData
{
    vec3 DirectionWS;
    vec3 Color;
};

struct LightPoint_PointLightDataInternal
{
    vec3 PositionWS;
    float InvSquareRadius;
    vec3 Color;
};

struct LightSpot_SpotLightDataInternal
{
    vec3 PositionWS;
    vec3 DirectionWS;
    vec3 AngleOffsetAndInvSquareRadius;
    vec3 Color;
};

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    bool IsFrontFace_id1;
    vec4 ColorTarget_id2;
    vec3 meshNormal_id3;
    vec3 meshNormalWS_id4;
    vec4 meshTangent_id5;
    vec3 normalWS_id6;
    mat3 tangentToWorld_id7;
    vec4 PositionWS_id8;
    float DepthVS_id9;
    vec3 matNormal_id10;
    vec4 matColorBase_id11;
    vec4 matDiffuse_id12;
    float matGlossiness_id13;
    vec3 matSpecular_id14;
    float matSpecularIntensity_id15;
    float matAmbientOcclusion_id16;
    float matAmbientOcclusionDirectLightingFactor_id17;
    float matCavity_id18;
    float matCavityDiffuse_id19;
    float matCavitySpecular_id20;
    vec4 matEmissive_id21;
    float matEmissiveIntensity_id22;
    float matScatteringStrength_id23;
    vec2 matDiffuseSpecularAlphaBlend_id24;
    vec3 matAlphaBlendColor_id25;
    float matAlphaDiscard_id26;
    vec3 viewWS_id27;
    vec3 matDiffuseVisible_id28;
    float alphaRoughness_id29;
    vec3 matSpecularVisible_id30;
    float NdotV_id31;
    vec3 shadingColor_id32;
    float shadingColorAlpha_id33;
    vec3 H_id34;
    float NdotH_id35;
    float LdotH_id36;
    float VdotH_id37;
    vec3 lightPositionWS_id38;
    vec3 lightDirectionWS_id39;
    vec3 lightColor_id40;
    vec3 lightColorNdotL_id41;
    vec3 envLightDiffuseColor_id42;
    vec3 envLightSpecularColor_id43;
    float NdotL_id44;
    float lightDirectAmbientOcclusion_id45;
    float matBlend_id46;
    vec3 shadowColor_id47;
    float thicknessWS_id48;
    vec2 TexCoord_id49;
    uvec2 lightData_id50;
    int lightIndex_id51;
    vec4 ScreenPosition_id52;
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
    float o1S433C0_Camera_NearClipPlane;
    float o1S433C0_Camera_FarClipPlane;
    vec2 o1S433C0_Camera_ZProjection;
    vec2 o1S433C0_Camera_ViewSize;
    float o1S433C0_Camera_AspectRatio;
    vec4 o0S433C0_ShadowMapReceiverDirectional_padding_PerView_Default;
    float o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[4];
    layout(row_major) mat4 o0S433C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[4];
    layout(row_major) mat4 o0S433C0_ShadowMapReceiverBase_InverseWorldToShadowCascadeUV[4];
    layout(row_major) mat4 o0S433C0_ShadowMapReceiverBase_ViewMatrices[4];
    vec2 o0S433C0_ShadowMapReceiverBase_DepthRanges[4];
    float o0S433C0_ShadowMapReceiverBase_DepthBiases[1];
    float o0S433C0_ShadowMapReceiverBase_OffsetScales[1];
    vec2 o0S433C0_ShadowMapCommon_ShadowMapTextureSize;
    vec2 o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize;
    LightDirectional_DirectionalLightData o0S433C0_LightDirectionalGroup_Lights[1];
    int o0S433C0_DirectLightGroupPerView_LightCount;
    float o1S433C0_LightClustered_ClusterDepthScale;
    float o1S433C0_LightClustered_ClusterDepthBias;
    vec2 o1S433C0_LightClustered_ClusterStride;
    vec3 o3S417C0_LightSimpleAmbient_AmbientLight;
    layout(row_major) mat4 o6S417C0_LightSkyboxShader_SkyMatrix;
    float o6S417C0_LightSkyboxShader_Intensity;
    float o6S417C0_o5S5C1_RoughnessCubeMapEnvironmentColor_MipCount;
    vec3 o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[9];
    vec4 o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_padding_PerView_Lighting;
} PerView_var;

layout(std140) uniform PerMaterial
{
    float o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec4 o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_constantColor;
    vec2 o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec4 o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_constantColor;
    float o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    vec4 o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
} PerMaterial_var;

uniform samplerBuffer LightClusteredPointGroup_PointLights;
uniform usamplerBuffer LightClustered_LightIndices;
uniform samplerBuffer LightClusteredSpotGroup_SpotLights;
uniform sampler2D SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler;
uniform sampler2DShadow SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureTexturing_LinearClampCompareLessEqualSampler;
uniform sampler2D SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureTexturing_LinearBorderSampler;
uniform usampler3D SPIRV_Cross_CombinedLightClustered_LightClustersSPIRV_Cross_DummySampler;
uniform sampler2D SPIRV_Cross_CombinedMaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUTTexturing_LinearSampler;
uniform samplerCube SPIRV_Cross_CombinedRoughnessCubeMapEnvironmentColor_CubeMapTexturing_LinearSampler;

in vec4 PS_IN_SV_Position;
in vec3 PS_IN_NORMAL;
in vec4 PS_IN_TANGENT;
in vec4 PS_IN_POSITION_WS;
in float PS_IN_DEPTH_VS;
in vec2 PS_IN_TEXCOORD0;
in vec4 PS_IN_SCREENPOSITION;
in bool PS_IN_SV_IsFrontFace;
out vec4 PS_OUT_ColorTarget;

float _1509;

void NormalUpdate_GenerateNormal_PS()
{
}

mat3 NormalUpdate_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id3 = normalize(_streams.meshNormal_id3);
    vec3 tangent = normalize(_streams.meshTangent_id5.xyz);
    vec3 bitangent = cross(_streams.meshNormal_id3, tangent) * _streams.meshTangent_id5.w;
    mat3 tangentMatrix = mat3(vec3(tangent), vec3(bitangent), vec3(_streams.meshNormal_id3));
    return tangentMatrix;
}

mat3 NormalFromNormalMapping_GetTangentWorldTransform()
{
    return mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z));
}

void NormalUpdate_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    mat3 _114 = NormalUpdate_GetTangentMatrix(_streams);
    mat3 tangentMatrix = _114;
    mat3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id7 = tangentWorldTransform * tangentMatrix;
}

void NormalFromNormalMapping_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    NormalUpdate_GenerateNormal_PS();
    NormalUpdate_UpdateTangentToWorld(_streams);
    _streams.meshNormalWS_id4 = mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z)) * _streams.meshNormal_id3;
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
    _streams.matBlend_id46 = 0.0;
}

void o43S247C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o43S247C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id10 = vec3(0.0, 0.0, 1.0);
    _streams.matColorBase_id11 = vec4(0.0);
    _streams.matDiffuse_id12 = vec4(0.0);
    _streams.matDiffuseVisible_id28 = vec3(0.0);
    _streams.matSpecular_id14 = vec3(0.0);
    _streams.matSpecularVisible_id30 = vec3(0.0);
    _streams.matSpecularIntensity_id15 = 1.0;
    _streams.matGlossiness_id13 = 0.0;
    _streams.alphaRoughness_id29 = 1.0;
    _streams.matAmbientOcclusion_id16 = 1.0;
    _streams.matAmbientOcclusionDirectLightingFactor_id17 = 0.0;
    _streams.matCavity_id18 = 1.0;
    _streams.matCavityDiffuse_id19 = 0.0;
    _streams.matCavitySpecular_id20 = 0.0;
    _streams.matEmissive_id21 = vec4(0.0);
    _streams.matEmissiveIntensity_id22 = 0.0;
    _streams.matScatteringStrength_id23 = 1.0;
    _streams.matDiffuseSpecularAlphaBlend_id24 = vec2(1.0);
    _streams.matAlphaBlendColor_id25 = vec3(1.0);
    _streams.matAlphaDiscard_id26 = 0.100000001490116119384765625;
}

void o43S247C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o43S247C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id33 = 1.0;
}

vec4 o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return PerMaterial_var.o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
}

void o42S247C0_o9S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o42S247C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id12 = colorBase;
    _streams.matColorBase_id11 = colorBase;
}

vec4 o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id49 * PerMaterial_var.o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o42S247C0_o11S2C0_MaterialSurfaceNormalMap_true_true__Compute(inout PS_STREAMS _streams)
{
    vec4 normal = o42S247C0_o11S2C0_o10S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0) - vec4(1.0);
    }
    if (true)
    {
        normal.z = sqrt(max(0.0, 1.0 - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id10 = normal.xyz;
}

vec4 o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return vec4(PerMaterial_var.o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o13S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o42S247C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0 - glossiness;
    }
    _streams.matGlossiness_id13 = glossiness;
}

vec4 o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return PerMaterial_var.o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_constantColor;
}

void o42S247C0_o15S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecular_id14 = o42S247C0_o15S2C0_o14S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

vec4 o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return vec4(PerMaterial_var.o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o17S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id15 = o42S247C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

vec4 o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i1_Material_TextureOffset_i1__Compute(PS_STREAMS _streams)
{
    return vec4(texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id49 * PerMaterial_var.o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset).x);
}

void o42S247C0_o19S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusion_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusion_id16 = o42S247C0_o19S2C0_o18S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i1_Material_TextureOffset_i1__Compute(_streams).x;
}

vec4 o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue__Compute()
{
    return vec4(PerMaterial_var.o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o21S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusionDirectLightingFactor_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusionDirectLightingFactor_id17 = o42S247C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue__Compute().x;
}

vec4 o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_Material_CavityValue__Compute()
{
    return vec4(PerMaterial_var.o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o23S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavity_id18 = o42S247C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_Material_CavityValue__Compute().x;
}

vec4 o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue__Compute()
{
    return vec4(PerMaterial_var.o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o25S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavityDiffuse_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavityDiffuse_id19 = o42S247C0_o25S2C0_o24S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue__Compute().x;
}

vec4 o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue__Compute()
{
    return vec4(PerMaterial_var.o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o27S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavitySpecular_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavitySpecular_id20 = o42S247C0_o27S2C0_o26S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue__Compute().x;
}

vec4 o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_EmissiveMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_i2_Material_TextureOffset_i2__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id49 * PerMaterial_var.o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

vec4 o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_Material_EmissiveValue__Compute()
{
    return PerMaterial_var.o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_constantColor;
}

vec4 o42S247C0_o31S2C0_o30S2C0_ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    vec4 tex1 = o42S247C0_o31S2C0_o30S2C0_o28S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_EmissiveMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_i2_Material_TextureOffset_i2__Compute(_streams);
    vec4 tex2 = o42S247C0_o31S2C0_o30S2C0_o29S2C1_ComputeColorConstantColorLink_Material_EmissiveValue__Compute();
    vec4 mix1 = tex1 * tex2;
    return mix1;
}

void o42S247C0_o31S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissive_rgba__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissive_id21 = o42S247C0_o31S2C0_o30S2C0_ComputeColorMultiply_Compute(_streams);
}

vec4 o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute()
{
    return vec4(PerMaterial_var.o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o42S247C0_o33S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissiveIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissiveIntensity_id22 = o42S247C0_o33S2C0_o32S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute().x;
}

void NormalUpdate_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id6 = normalize(_streams.tangentToWorld_id7 * normalInTangentSpace);
}

void o42S247C0_o40S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id38 = vec3(0.0);
    _streams.lightDirectionWS_id39 = vec3(0.0);
    _streams.lightColor_id40 = vec3(0.0);
    _streams.lightColorNdotL_id41 = vec3(0.0);
    _streams.envLightDiffuseColor_id42 = vec3(0.0);
    _streams.envLightSpecularColor_id43 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id45 = 1.0;
    _streams.NdotL_id44 = 0.0;
}

void o42S247C0_o40S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id45 = mix(1.0, _streams.matAmbientOcclusion_id16, _streams.matAmbientOcclusionDirectLightingFactor_id17);
    _streams.matDiffuseVisible_id28 = ((_streams.matDiffuse_id12.xyz * mix(1.0, _streams.matCavity_id18, _streams.matCavityDiffuse_id19)) * _streams.matDiffuseSpecularAlphaBlend_id24.x) * _streams.matAlphaBlendColor_id25;
    _streams.matSpecularVisible_id30 = (((_streams.matSpecular_id14 * _streams.matSpecularIntensity_id15) * mix(1.0, _streams.matCavity_id18, _streams.matCavitySpecular_id20)) * _streams.matDiffuseSpecularAlphaBlend_id24.y) * _streams.matAlphaBlendColor_id25;
    _streams.NdotV_id31 = max(dot(_streams.normalWS_id6, _streams.viewWS_id27), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id13;
    _streams.alphaRoughness_id29 = max(roughness * roughness, 0.001000000047497451305389404296875);
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
    return PerView_var.o0S433C0_DirectLightGroupPerView_LightCount;
}

void o0S433C0_LightDirectionalGroup_1__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id40 = PerView_var.o0S433C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id39 = -PerView_var.o0S433C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

vec3 o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__GetShadowPositionOffset(float offsetScale, float nDotL, vec3 normal)
{
    float normalOffsetScale = clamp(1.0 - nDotL, 0.0, 1.0);
    return normal * (((2.0 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize.x) * offsetScale) * normalOffsetScale);
}

void o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__CalculatePCFKernelParameters(vec2 position, inout vec2 base_uv, out vec2 st)
{
    vec2 uv = position * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureSize;
    base_uv = floor(uv + vec2(0.5));
    st = (uv + vec2(0.5)) - base_uv;
    base_uv -= vec2(0.5);
    base_uv *= PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize;
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get3x3FilterKernel(vec2 base_uv, vec2 st, inout vec3 kernel[4])
{
    vec2 uvW0 = vec2(3.0) - (st * 2.0);
    vec2 uvW1 = vec2(1.0) + (st * 2.0);
    vec2 uv0 = ((vec2(2.0) - st) / uvW0) - vec2(1.0);
    vec2 uv1 = (st / uvW1) + vec2(1.0);
    kernel[0] = vec3(base_uv + (uv0 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW0.y);
    kernel[1] = vec3(base_uv + (vec2(uv1.x, uv0.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW0.y);
    kernel[2] = vec3(base_uv + (vec2(uv0.x, uv1.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW1.y);
    kernel[3] = vec3(base_uv + (uv1 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW1.y);
    return 16.0;
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(vec2 position, float positionDepth)
{
    vec3 _1367 = vec3(position, positionDepth);
    return textureLod(SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureTexturing_LinearClampCompareLessEqualSampler, vec3(_1367.xy, _1367.z), 0.0);
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get5x5FilterKernel(vec2 base_uv, vec2 st, inout vec3 kernel[9])
{
    vec2 uvW0 = vec2(4.0) - (st * 3.0);
    vec2 uvW1 = vec2(7.0);
    vec2 uvW2 = vec2(1.0) + (st * 3.0);
    vec2 uv0 = ((vec2(3.0) - (st * 2.0)) / uvW0) - vec2(2.0);
    vec2 uv1 = (vec2(3.0) + st) / uvW1;
    vec2 uv2 = (st / uvW2) + vec2(2.0);
    kernel[0] = vec3(base_uv + (uv0 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW0.y);
    kernel[1] = vec3(base_uv + (vec2(uv1.x, uv0.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW0.y);
    kernel[2] = vec3(base_uv + (vec2(uv2.x, uv0.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW0.y);
    kernel[3] = vec3(base_uv + (vec2(uv0.x, uv1.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW1.y);
    kernel[4] = vec3(base_uv + (uv1 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW1.y);
    kernel[5] = vec3(base_uv + (vec2(uv2.x, uv1.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW1.y);
    kernel[6] = vec3(base_uv + (vec2(uv0.x, uv2.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW2.y);
    kernel[7] = vec3(base_uv + (vec2(uv1.x, uv2.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW2.y);
    kernel[8] = vec3(base_uv + (uv2 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW2.y);
    return 144.0;
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get7x7FilterKernel(vec2 base_uv, vec2 st, inout vec3 kernel[16])
{
    vec2 uvW0 = (st * 5.0) - vec2(6.0);
    vec2 uvW1 = (st * 11.0) - vec2(28.0);
    vec2 uvW2 = -((st * 11.0) + vec2(17.0));
    vec2 uvW3 = -((st * 5.0) + vec2(1.0));
    vec2 uv0 = (((st * 4.0) - vec2(5.0)) / uvW0) - vec2(3.0);
    vec2 uv1 = (((st * 4.0) - vec2(16.0)) / uvW1) - vec2(1.0);
    vec2 uv2 = ((-((st * 7.0) + vec2(5.0))) / uvW2) + vec2(1.0);
    vec2 uv3 = ((-st) / uvW3) + vec2(3.0);
    kernel[0] = vec3(base_uv + (uv0 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW0.y);
    kernel[1] = vec3(base_uv + (vec2(uv1.x, uv0.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW0.y);
    kernel[2] = vec3(base_uv + (vec2(uv2.x, uv0.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW0.y);
    kernel[3] = vec3(base_uv + (vec2(uv3.x, uv0.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW0.y);
    kernel[4] = vec3(base_uv + (vec2(uv0.x, uv1.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW1.y);
    kernel[5] = vec3(base_uv + (uv1 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW1.y);
    kernel[6] = vec3(base_uv + (vec2(uv2.x, uv1.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW1.y);
    kernel[7] = vec3(base_uv + (vec2(uv3.x, uv1.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW1.y);
    kernel[8] = vec3(base_uv + (vec2(uv0.x, uv2.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW2.y);
    kernel[9] = vec3(base_uv + (vec2(uv1.x, uv2.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW2.y);
    kernel[10] = vec3(base_uv + (uv2 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW2.y);
    kernel[11] = vec3(base_uv + (vec2(uv3.x, uv2.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW2.y);
    kernel[12] = vec3(base_uv + (vec2(uv0.x, uv3.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW0.x * uvW3.y);
    kernel[13] = vec3(base_uv + (vec2(uv1.x, uv3.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW1.x * uvW3.y);
    kernel[14] = vec3(base_uv + (vec2(uv2.x, uv3.y) * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW2.x * uvW3.y);
    kernel[15] = vec3(base_uv + (uv3 * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize), uvW3.x * uvW3.y);
    return 2704.0;
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterShadow(vec2 position, float positionDepth)
{
    float shadow = 0.0;
    vec2 param = position;
    vec2 param_1;
    vec2 param_2;
    o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__CalculatePCFKernelParameters(param, param_1, param_2);
    vec2 base_uv = param_1;
    vec2 st = param_2;
    if (false)
    {
        vec2 param_3 = base_uv;
        vec2 param_4 = st;
        vec3 param_5[4];
        float _1411 = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get3x3FilterKernel(param_3, param_4, param_5);
        vec3 kernel[4] = param_5;
        float normalizationFactor = _1411;
        for (int i = 0; i < 4; i++)
        {
            vec2 param_6 = kernel[i].xy;
            float param_7 = positionDepth;
            shadow += (kernel[i].z * o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(param_6, param_7));
        }
        shadow /= normalizationFactor;
    }
    else
    {
        if (true)
        {
            vec2 param_8 = base_uv;
            vec2 param_9 = st;
            vec3 param_10[9];
            float _1441 = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get5x5FilterKernel(param_8, param_9, param_10);
            vec3 kernel_1[9] = param_10;
            float normalizationFactor_1 = _1441;
            for (int i_1 = 0; i_1 < 9; i_1++)
            {
                vec2 param_11 = kernel_1[i_1].xy;
                float param_12 = positionDepth;
                shadow += (kernel_1[i_1].z * o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(param_11, param_12));
            }
            shadow /= normalizationFactor_1;
        }
        else
        {
            if (false)
            {
                vec2 param_13 = base_uv;
                vec2 param_14 = st;
                vec3 param_15[16];
                float _1471 = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get7x7FilterKernel(param_13, param_14, param_15);
                vec3 kernel_2[16] = param_15;
                float normalizationFactor_2 = _1471;
                for (int i_2 = 0; i_2 < 16; i_2++)
                {
                    vec2 param_16 = kernel_2[i_2].xy;
                    float param_17 = positionDepth;
                    shadow += (kernel_2[i_2].z * o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleTextureAndCompare(param_16, param_17));
                }
                shadow /= normalizationFactor_2;
            }
        }
    }
    return shadow;
}

float o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(vec3 shadowPositionWS, int cascadeIndex, int lightIndex)
{
    vec4 shadowPosition = (PerView_var.o0S433C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[cascadeIndex + (lightIndex * 4)]) * vec4(shadowPositionWS, 1.0);
    shadowPosition.z -= PerView_var.o0S433C0_ShadowMapReceiverBase_DepthBiases[lightIndex];
    vec3 _476 = shadowPosition.xyz / vec3(shadowPosition.w);
    shadowPosition = vec4(_476.x, _476.y, _476.z, shadowPosition.w);
    vec2 param = shadowPosition.xy;
    float param_1 = shadowPosition.z;
    return o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterShadow(param, param_1);
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__GetFilterRadiusInPixels()
{
    if (false)
    {
        return 2.5;
    }
    else
    {
        if (true)
        {
            return 3.5;
        }
        else
        {
            return 4.5;
        }
    }
}

vec4 o0S433C0_Math_Project(vec4 vec, mat4 mat)
{
    vec4 vecProjected = mat * vec;
    vec3 _683 = vecProjected.xyz / vec3(vecProjected.w);
    vecProjected = vec4(_683.x, _683.y, _683.z, vecProjected.w);
    return vecProjected;
}

void o0S433C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPosition(float filterRadiusInPixels, vec3 pixelPositionWS, vec3 meshNormalWS, mat4 worldToShadowCascadeUV, mat4 inverseWorldToShadowCascadeUV, inout vec3 adjustedPixelPositionWS, out vec3 adjustedPixelPositionShadowSpace)
{
    vec4 param = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 param_1 = inverseWorldToShadowCascadeUV;
    vec4 bottomLeftTexelWS = o0S433C0_Math_Project(param, param_1);
    vec4 param_2 = vec4(PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize * filterRadiusInPixels, 0.0, 1.0);
    mat4 param_3 = inverseWorldToShadowCascadeUV;
    vec4 topRightTexelWS = o0S433C0_Math_Project(param_2, param_3);
    float texelDiagonalLength = distance(topRightTexelWS.xyz, bottomLeftTexelWS.xyz);
    vec3 positionOffsetWS = meshNormalWS * texelDiagonalLength;
    adjustedPixelPositionWS = pixelPositionWS - positionOffsetWS;
    vec4 param_4 = vec4(adjustedPixelPositionWS, 1.0);
    mat4 param_5 = worldToShadowCascadeUV;
    vec4 shadowMapCoordinate = o0S433C0_Math_Project(param_4, param_5);
    adjustedPixelPositionShadowSpace = shadowMapCoordinate.xyz;
}

void o0S433C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPositionPerspective(float filterRadiusInPixels, vec3 pixelPositionWS, vec3 meshNormalWS, mat4 worldToShadowCascadeUV, mat4 inverseWorldToShadowCascadeUV, inout vec3 adjustedPixelPositionWS, out vec3 adjustedPixelPositionShadowSpace)
{
    vec4 param = vec4(pixelPositionWS, 1.0);
    mat4 param_1 = worldToShadowCascadeUV;
    vec4 shadowMapCoordinate = o0S433C0_Math_Project(param, param_1);
    vec4 param_2 = vec4(shadowMapCoordinate.xy + (PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize * filterRadiusInPixels), shadowMapCoordinate.z, 1.0);
    mat4 param_3 = inverseWorldToShadowCascadeUV;
    vec4 topRightTexelWS = o0S433C0_Math_Project(param_2, param_3);
    float texelDiagonalLength = distance(topRightTexelWS.xyz, pixelPositionWS);
    vec3 positionOffsetWS = meshNormalWS * texelDiagonalLength;
    adjustedPixelPositionWS = pixelPositionWS - positionOffsetWS;
    vec4 param_4 = vec4(adjustedPixelPositionWS, 1.0);
    mat4 param_5 = worldToShadowCascadeUV;
    vec4 adjustedShadowMapCoordinate = o0S433C0_Math_Project(param_4, param_5);
    adjustedPixelPositionShadowSpace = adjustedShadowMapCoordinate.xyz;
}

float o0S433C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(vec3 shadowSpaceCoordinate, vec3 pixelPositionWS, vec2 depthRanges, mat4 inverseWorldToShadowCascadeUV, bool isOrthographic)
{
    float shadowMapDepth = textureLod(SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureTexturing_LinearBorderSampler, shadowSpaceCoordinate.xy, 0.0).x;
    float thickness;
    if (isOrthographic)
    {
        thickness = abs(shadowMapDepth - shadowSpaceCoordinate.z) * depthRanges.y;
    }
    else
    {
        vec4 param = vec4(shadowSpaceCoordinate.xy, shadowMapDepth, 1.0);
        mat4 param_1 = inverseWorldToShadowCascadeUV;
        vec4 shadowmapPositionWorldSpace = o0S433C0_Math_Project(param, param_1);
        thickness = distance(shadowmapPositionWorldSpace.xyz, pixelPositionWS);
    }
    return thickness;
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleAndFilter(vec3 adjustedPixelPositionWS, vec3 adjustedPixelPositionShadowSpace, vec2 depthRanges, mat4 inverseWorldToShadowCascadeUV, bool isOrthographic, bool isDualParaboloid)
{
    vec2 uv = adjustedPixelPositionShadowSpace.xy * PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureSize;
    vec2 base_uv = floor(uv + vec2(0.5));
    vec2 st = (uv + vec2(0.5)) - base_uv;
    base_uv *= PerView_var.o0S433C0_ShadowMapCommon_ShadowMapTextureTexelSize;
    float thickness = 0.0;
    float normalizationFactor = 1.0;
    if (false)
    {
        vec2 param = base_uv;
        vec2 param_1 = st;
        vec3 param_2[4];
        float _1574 = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get3x3FilterKernel(param, param_1, param_2);
        vec3 kernel[4] = param_2;
        normalizationFactor = _1574;
        for (int i = 0; i < 4; i++)
        {
            vec3 param_3 = vec3(kernel[i].xy, adjustedPixelPositionShadowSpace.z);
            vec3 param_4 = adjustedPixelPositionWS;
            vec2 param_5 = depthRanges;
            mat4 param_6 = inverseWorldToShadowCascadeUV;
            bool param_7 = isOrthographic;
            thickness += (kernel[i].z * o0S433C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(param_3, param_4, param_5, param_6, param_7));
        }
    }
    else
    {
        if (true)
        {
            vec2 param_8 = base_uv;
            vec2 param_9 = st;
            vec3 param_10[9];
            float _1609 = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get5x5FilterKernel(param_8, param_9, param_10);
            vec3 kernel_1[9] = param_10;
            normalizationFactor = _1609;
            for (int i_1 = 0; i_1 < 9; i_1++)
            {
                vec3 param_11 = vec3(kernel_1[i_1].xy, adjustedPixelPositionShadowSpace.z);
                vec3 param_12 = adjustedPixelPositionWS;
                vec2 param_13 = depthRanges;
                mat4 param_14 = inverseWorldToShadowCascadeUV;
                bool param_15 = isOrthographic;
                thickness += (kernel_1[i_1].z * o0S433C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(param_11, param_12, param_13, param_14, param_15));
            }
        }
        else
        {
            if (false)
            {
                vec2 param_16 = base_uv;
                vec2 param_17 = st;
                vec3 param_18[16];
                float _1644 = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__Get7x7FilterKernel(param_16, param_17, param_18);
                vec3 kernel_2[16] = param_18;
                normalizationFactor = _1644;
                for (int i_2 = 0; i_2 < 16; i_2++)
                {
                    vec3 param_19 = vec3(kernel_2[i_2].xy, adjustedPixelPositionShadowSpace.z);
                    vec3 param_20 = adjustedPixelPositionWS;
                    vec2 param_21 = depthRanges;
                    mat4 param_22 = inverseWorldToShadowCascadeUV;
                    bool param_23 = isOrthographic;
                    thickness += (kernel_2[i_2].z * o0S433C0_ShadowMapFilterBase_PerView_Lighting__SampleThickness(param_19, param_20, param_21, param_22, param_23));
                }
            }
        }
    }
    return thickness / normalizationFactor;
}

float o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterThickness(vec3 pixelPositionWS, vec3 meshNormalWS, vec2 depthRanges, mat4 worldToShadowCascadeUV, mat4 inverseWorldToShadowCascadeUV, bool isOrthographic)
{
    vec3 adjustedPixelPositionWS;
    vec3 adjustedPixelPositionShadowSpace;
    if (isOrthographic)
    {
        float param = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__GetFilterRadiusInPixels();
        vec3 param_1 = pixelPositionWS;
        vec3 param_2 = meshNormalWS;
        mat4 param_3 = worldToShadowCascadeUV;
        mat4 param_4 = inverseWorldToShadowCascadeUV;
        vec3 param_5;
        vec3 param_6;
        o0S433C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPosition(param, param_1, param_2, param_3, param_4, param_5, param_6);
        adjustedPixelPositionWS = param_5;
        adjustedPixelPositionShadowSpace = param_6;
    }
    else
    {
        float param_7 = o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__GetFilterRadiusInPixels();
        vec3 param_8 = pixelPositionWS;
        vec3 param_9 = meshNormalWS;
        mat4 param_10 = worldToShadowCascadeUV;
        mat4 param_11 = inverseWorldToShadowCascadeUV;
        vec3 param_12;
        vec3 param_13;
        o0S433C0_ShadowMapFilterBase_PerView_Lighting__CalculateAdjustedShadowSpacePixelPositionPerspective(param_7, param_8, param_9, param_10, param_11, param_12, param_13);
        adjustedPixelPositionWS = param_12;
        adjustedPixelPositionShadowSpace = param_13;
    }
    vec3 param_14 = adjustedPixelPositionWS;
    vec3 param_15 = adjustedPixelPositionShadowSpace;
    vec2 param_16 = depthRanges;
    mat4 param_17 = inverseWorldToShadowCascadeUV;
    bool param_18 = isOrthographic;
    bool param_19 = false;
    return o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__SampleAndFilter(param_14, param_15, param_16, param_17, param_18, param_19);
}

float o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(vec3 pixelPositionWS, vec3 meshNormalWS, int cascadeIndex, int lightIndex, bool isOrthographic)
{
    int arrayIndex = cascadeIndex + (lightIndex * 4);
    vec3 param = pixelPositionWS;
    vec3 param_1 = meshNormalWS;
    vec2 param_2 = PerView_var.o0S433C0_ShadowMapReceiverBase_DepthRanges[arrayIndex];
    mat4 param_3 = PerView_var.o0S433C0_ShadowMapReceiverBase_WorldToShadowCascadeUV[arrayIndex];
    mat4 param_4 = PerView_var.o0S433C0_ShadowMapReceiverBase_InverseWorldToShadowCascadeUV[arrayIndex];
    bool param_5 = isOrthographic;
    return o0S433C0_ShadowMapFilterPcf_PerView_Lighting_5__FilterThickness(param, param_1, param_2, param_3, param_4, param_5);
}

vec3 o0S433C0_ShadowMapReceiverDirectional_4_1_true_true_false_false__ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    int cascadeIndexBase = lightIndex * 4;
    int cascadeIndex = 0;
    for (int i = 0; i < 3; i++)
    {
        if (_streams.DepthVS_id9 > (PerView_var.o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[cascadeIndexBase + i]))
        {
            cascadeIndex = i + 1;
        }
    }
    vec3 shadow = vec3(1.0);
    float tempThickness = 999.0;
    vec3 shadowPosition = position;
    float param = PerView_var.o0S433C0_ShadowMapReceiverBase_OffsetScales[lightIndex];
    float param_1 = _streams.NdotL_id44;
    vec3 param_2 = _streams.normalWS_id6;
    shadowPosition += o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__GetShadowPositionOffset(param, param_1, param_2);
    if (cascadeIndex < 4)
    {
        vec3 param_3 = shadowPosition;
        int param_4 = cascadeIndex;
        int param_5 = lightIndex;
        shadow = vec3(o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(param_3, param_4, param_5));
        if (false)
        {
            vec3 param_6 = _streams.PositionWS_id8.xyz;
            vec3 param_7 = _streams.meshNormalWS_id4;
            int param_8 = cascadeIndex;
            int param_9 = lightIndex;
            bool param_10 = true;
            tempThickness = o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(param_6, param_7, param_8, param_9, param_10);
        }
        float nextSplit = PerView_var.o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[cascadeIndexBase + cascadeIndex];
        float splitSize = nextSplit;
        if (cascadeIndex > 0)
        {
            splitSize = nextSplit - (PerView_var.o0S433C0_ShadowMapReceiverDirectional_CascadeDepthSplits[(cascadeIndexBase + cascadeIndex) - 1]);
        }
        float splitDist = (nextSplit - _streams.DepthVS_id9) / splitSize;
        if (splitDist < 0.20000000298023223876953125)
        {
            float lerpAmt = smoothstep(0.0, 0.20000000298023223876953125, splitDist);
            if (cascadeIndex == 3)
            {
                if (false)
                {
                    shadow = mix(vec3(1.0), shadow, vec3(lerpAmt));
                    if (false)
                    {
                        tempThickness = mix(0.0, tempThickness, lerpAmt);
                    }
                }
            }
            else
            {
                if (true)
                {
                    vec3 param_11 = shadowPosition;
                    int param_12 = cascadeIndex + 1;
                    int param_13 = lightIndex;
                    float nextShadow = o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeShadowFromCascade(param_11, param_12, param_13);
                    shadow = mix(vec3(nextShadow), shadow, vec3(lerpAmt));
                    if (false)
                    {
                        vec3 param_14 = _streams.PositionWS_id8.xyz;
                        vec3 param_15 = _streams.meshNormalWS_id4;
                        int param_16 = cascadeIndex + 1;
                        int param_17 = lightIndex;
                        bool param_18 = true;
                        float nextThickness = o0S433C0_ShadowMapReceiverBase_PerView_Lighting_4_1__ComputeThicknessFromCascade(param_14, param_15, param_16, param_17, param_18);
                        tempThickness = mix(nextThickness, tempThickness, lerpAmt);
                    }
                }
            }
        }
    }
    _streams.thicknessWS_id48 = tempThickness;
    if (false)
    {
        vec3 indexable[5] = vec3[](vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), vec3(1.0));
        return indexable[cascadeIndex] * shadow;
    }
    return shadow;
}

void o0S433C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S433C0_LightDirectionalGroup_1__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id8.xyz;
    int param_2 = lightIndex;
    vec3 _239 = o0S433C0_ShadowMapReceiverDirectional_4_1_true_true_false_false__ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id47 = _239;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id47) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id34 = normalize(_streams.viewWS_id27 + _streams.lightDirectionWS_id39);
    _streams.NdotH_id35 = clamp(dot(_streams.normalWS_id6, _streams.H_id34), 0.0, 1.0);
    _streams.LdotH_id36 = clamp(dot(_streams.lightDirectionWS_id39, _streams.H_id34), 0.0, 1.0);
    _streams.VdotH_id37 = _streams.LdotH_id36;
}

vec3 o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id30);
    }
    return ((diffuseColor / vec3(3.1415927410125732421875)) * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.x;
}

vec3 o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, vec3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0 - lOrVDotH, 5.0));
}

vec3 o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, float lOrVDotH)
{
    vec3 param = f0;
    vec3 param_1 = vec3(1.0);
    float param_2 = lOrVDotH;
    return o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

vec3 o42S247C0_o40S2C0_o39S2C0_o36S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, vec3 f0)
{
    vec3 param = f0;
    float param_1 = _streams.LdotH_id36;
    return o42S247C0_o40S2C0_o39S2C0_o36S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o42S247C0_o40S2C0_o39S2C0_o37S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5;
    return nDotX / ((nDotX * (1.0 - k)) + k);
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
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0)) + 1.0, 9.9999997473787516355514526367188e-05);
    return alphaR2 / ((3.1415927410125732421875 * d) * d);
}

float o42S247C0_o40S2C0_o39S2C0_o38S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id29;
    float param_1 = _streams.NdotH_id35;
    return o42S247C0_o40S2C0_o39S2C0_o38S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

vec3 o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id30;
    vec3 param = specularColor;
    vec3 fresnel = o42S247C0_o40S2C0_o39S2C0_o36S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o42S247C0_o40S2C0_o39S2C0_o37S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o42S247C0_o40S2C0_o39S2C0_o38S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    vec3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / vec3(4.0);
    return (reflected * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.y;
}

void o1S433C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = PerView_var.o1S433C0_Camera_ZProjection.y / (projectedDepth - PerView_var.o1S433C0_Camera_ZProjection.x);
    vec2 texCoord = vec2(_streams.ScreenPosition_id52.x + 1.0, 1.0 - _streams.ScreenPosition_id52.y) * 0.5;
    int slice = int(max(log2((depth * PerView_var.o1S433C0_LightClustered_ClusterDepthScale) + PerView_var.o1S433C0_LightClustered_ClusterDepthBias), 0.0));
    _streams.lightData_id50 = uvec2(texelFetch(SPIRV_Cross_CombinedLightClustered_LightClustersSPIRV_Cross_DummySampler, ivec4(ivec2(texCoord * PerView_var.o1S433C0_LightClustered_ClusterStride), slice, 0).xyz, ivec4(ivec2(texCoord * PerView_var.o1S433C0_LightClustered_ClusterStride), slice, 0).w).xy);
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
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o1S433C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S433C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S433C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, vec3 position, out vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    float lightInvSquareRadius = light.InvSquareRadius;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S433C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S433C0_LightPoint_ProcessLight(inout PS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id8.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _1931 = o1S433C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _1931;
    _streams.lightPositionWS_id38 = light.PositionWS;
    _streams.lightColor_id40 = light.Color * attenuation;
    _streams.lightDirectionWS_id39 = lightVectorNorm;
}

void o1S433C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id51).x);
    _streams.lightIndex_id51++;
    vec4 pointLight1 = texelFetch(LightClusteredPointGroup_PointLights, realLightIndex * 2);
    vec4 pointLight2 = texelFetch(LightClusteredPointGroup_PointLights, (realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S433C0_LightPoint_ProcessLight(_streams, param);
}

vec3 o1S433C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id48 = 0.0;
    return vec3(1.0);
}

void o1S433C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S433C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id8.xyz;
    int param_2 = lightIndex;
    vec3 _1847 = o1S433C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id47 = _1847;
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
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o2S433C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S433C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S433C0_LightUtil_GetAngleAttenuation(vec3 lightVector, vec3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0, 1.0);
    attenuation *= attenuation;
    return attenuation;
}

float o2S433C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, vec3 position, inout vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    vec3 lightAngleOffsetAndInvSquareRadius = light.AngleOffsetAndInvSquareRadius;
    vec2 lightAngleAndOffset = lightAngleOffsetAndInvSquareRadius.xy;
    float lightInvSquareRadius = lightAngleOffsetAndInvSquareRadius.z;
    vec3 lightDirection = -light.DirectionWS;
    float attenuation = 1.0;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S433C0_LightUtil_GetDistanceAttenuation(param, param_1);
    vec3 param_2 = lightVectorNorm;
    vec3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S433C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S433C0_LightSpot_ProcessLight(inout PS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id8.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _2213 = o2S433C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _2213;
    _streams.lightColor_id40 = light.Color * attenuation;
    _streams.lightDirectionWS_id39 = lightVectorNorm;
}

void o2S433C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id51).x);
    _streams.lightIndex_id51++;
    vec4 spotLight1 = texelFetch(LightClusteredSpotGroup_SpotLights, realLightIndex * 4);
    vec4 spotLight2 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 1);
    vec4 spotLight3 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 2);
    vec4 spotLight4 = texelFetch(LightClusteredSpotGroup_SpotLights, (realLightIndex * 4) + 3);
    LightSpot_SpotLightDataInternal spotLight;
    spotLight.PositionWS = spotLight1.xyz;
    spotLight.DirectionWS = spotLight2.xyz;
    spotLight.AngleOffsetAndInvSquareRadius = spotLight3.xyz;
    spotLight.Color = spotLight4.xyz;
    LightSpot_SpotLightDataInternal param = spotLight;
    o2S433C0_LightSpot_ProcessLight(_streams, param);
}

vec3 o2S433C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id48 = 0.0;
    return vec3(1.0);
}

void o2S433C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S433C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id8.xyz;
    int param_2 = lightIndex;
    vec3 _2117 = o2S433C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id47 = _2117;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id47) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void o3S417C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = vec3(0.0);
    _streams.envLightSpecularColor_id43 = vec3(0.0);
}

void o3S417C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S417C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    vec3 lightColor = PerView_var.o3S417C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id16;
    _streams.envLightDiffuseColor_id42 = lightColor;
    _streams.envLightSpecularColor_id43 = lightColor;
}

vec3 o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id30);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id42;
}

vec3 o42S247C0_o40S2C0_o39S2C0_o35S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(vec3 specularColor, float alphaR, float nDotV)
{
    float glossiness = 1.0 - sqrt(alphaR);
    vec4 environmentLightingDFG = textureLod(SPIRV_Cross_CombinedMaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUTTexturing_LinearSampler, vec2(glossiness, nDotV), 0.0);
    return (specularColor * environmentLightingDFG.x) + vec3(environmentLightingDFG.y);
}

vec3 o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id30;
    vec3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id29;
    float param_2 = _streams.NdotV_id31;
    return o42S247C0_o40S2C0_o39S2C0_o35S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(param, param_1, param_2) * _streams.envLightSpecularColor_id43;
}

void o6S417C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = vec3(0.0);
    _streams.envLightSpecularColor_id43 = vec3(0.0);
}

vec4 o6S417C0_o4S5C0_SphericalHarmonicsUtils_3__EvaluateSphericalHarmonics(vec3 sphericalColors[9], vec3 direction)
{
    float x = direction.x;
    float y = direction.y;
    float z = direction.z;
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    vec3 color = sphericalColors[0];
    if (true)
    {
        color += (sphericalColors[1] * y);
        color += (sphericalColors[2] * z);
        color += (sphericalColors[3] * x);
        if (true)
        {
            color += ((sphericalColors[4] * y) * x);
            color += ((sphericalColors[5] * y) * z);
            color += (sphericalColors[6] * ((3.0 * z2) - 1.0));
            color += ((sphericalColors[7] * x) * z);
            color += (sphericalColors[8] * (x2 - y2));
            if (false)
            {
                float z3 = z2 * z;
                float x4 = x2 * x2;
                float y4 = y2 * y2;
                float z4 = z2 * z2;
                color += ((sphericalColors[9] * y) * ((3.0 * x2) - y2));
                color += (((sphericalColors[10] * y) * x) * z);
                color += ((sphericalColors[11] * y) * ((-1.0) + (5.0 * z2)));
                color += (sphericalColors[12] * ((5.0 * z3) - (3.0 * z)));
                color += ((sphericalColors[13] * x) * ((-1.0) + (5.0 * z2)));
                color += ((sphericalColors[14] * (x2 - y2)) * z);
                color += ((sphericalColors[15] * x) * (x2 - (3.0 * y2)));
                if (false)
                {
                    color += (((sphericalColors[16] * x) * y) * (x2 - y2));
                    color += (((sphericalColors[17] * y) * z) * ((3.0 * x2) - y2));
                    color += (((sphericalColors[18] * y) * x) * ((-1.0) + (7.0 * z2)));
                    color += (((sphericalColors[19] * y) * z) * ((-3.0) + (7.0 * z2)));
                    color += (sphericalColors[20] * (((105.0 * z4) - (90.0 * z2)) + 9.0));
                    color += (((sphericalColors[21] * x) * z) * ((-3.0) + (7.0 * z2)));
                    color += ((sphericalColors[22] * (x2 - y2)) * ((-1.0) + (7.0 * z2)));
                    color += (((sphericalColors[23] * x) * z) * (x2 - (3.0 * y2)));
                    color += (sphericalColors[24] * ((x4 - ((6.0 * y2) * x2)) + y4));
                }
            }
        }
    }
    return vec4(color, 1.0);
}

vec4 o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_3__Compute(vec3 direction)
{
    vec3 param[9];
    param[0] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[0];
    param[1] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[1];
    param[2] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[2];
    param[3] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[3];
    param[4] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[4];
    param[5] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[5];
    param[6] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[6];
    param[7] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[7];
    param[8] = PerView_var.o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_SphericalColors[8];
    vec3 param_1 = direction;
    return o6S417C0_o4S5C0_SphericalHarmonicsUtils_3__EvaluateSphericalHarmonics(param, param_1);
}

vec4 o6S417C0_o5S5C1_RoughnessCubeMapEnvironmentColor_Compute(PS_STREAMS _streams, vec3 direction)
{
    float alpha = _streams.alphaRoughness_id29;
    float mipLevel = sqrt(alpha) * PerView_var.o6S417C0_o5S5C1_RoughnessCubeMapEnvironmentColor_MipCount;
    return textureLod(SPIRV_Cross_CombinedRoughnessCubeMapEnvironmentColor_CubeMapTexturing_LinearSampler, direction, mipLevel);
}

void o6S417C0_LightSkyboxShader_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o6S417C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float ambientAccessibility = _streams.matAmbientOcclusion_id16;
    vec3 sampleDirection = mat3(vec3(PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[0].x, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[0].y, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[0].z), vec3(PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[1].x, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[1].y, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[1].z), vec3(PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[2].x, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[2].y, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[2].z)) * _streams.normalWS_id6;
    sampleDirection = vec3(sampleDirection.xy, -sampleDirection.z);
    vec3 param = sampleDirection;
    _streams.envLightDiffuseColor_id42 = ((o6S417C0_o4S5C0_SphericalHarmonicsEnvironmentColor_3__Compute(param).xyz * PerView_var.o6S417C0_LightSkyboxShader_Intensity) * ambientAccessibility) * _streams.matDiffuseSpecularAlphaBlend_id24.x;
    sampleDirection = reflect(-_streams.viewWS_id27, _streams.normalWS_id6);
    sampleDirection = mat3(vec3(PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[0].x, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[0].y, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[0].z), vec3(PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[1].x, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[1].y, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[1].z), vec3(PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[2].x, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[2].y, PerView_var.o6S417C0_LightSkyboxShader_SkyMatrix[2].z)) * sampleDirection;
    sampleDirection = vec3(sampleDirection.xy, -sampleDirection.z);
    vec3 param_1 = sampleDirection;
    _streams.envLightSpecularColor_id43 = ((o6S417C0_o5S5C1_RoughnessCubeMapEnvironmentColor_Compute(_streams, param_1).xyz * PerView_var.o6S417C0_LightSkyboxShader_Intensity) * ambientAccessibility) * _streams.matDiffuseSpecularAlphaBlend_id24.y;
}

void o7S417C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = vec3(0.0);
    _streams.envLightSpecularColor_id43 = vec3(0.0);
}

void o42S247C0_o40S2C0_o34S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o42S247C0_o40S2C0_o39S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o42S247C0_o40S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id10;
    NormalUpdate_UpdateNormalFromTangentSpace(_streams, param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id6 = -_streams.normalWS_id6;
    }
    o42S247C0_o40S2C0_LightStream_ResetLightStream(_streams);
    o42S247C0_o40S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o42S247C0_o40S2C0_o34S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    o42S247C0_o40S2C0_o39S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    vec3 directLightingContribution = vec3(0.0);
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
    vec3 environmentLightingContribution = vec3(0.0);
    o3S417C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    o6S417C0_LightSkyboxShader_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    o7S417C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o34S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o42S247C0_o40S2C0_o39S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id32 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
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

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id27 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id8.xyz);
    _streams.shadingColor_id32 = vec3(0.0);
    o43S247C1_MaterialPixelShadingStream_ResetStream(_streams);
    o42S247C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id32, _streams.shadingColorAlpha_id33);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _13 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void NormalBase_PSMain(inout PS_STREAMS _streams)
{
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), false, vec4(0.0), vec3(0.0), vec3(0.0), vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), 0.0, vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), 0.0, vec2(0.0), uvec2(0u), 0, vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.meshNormal_id3 = PS_IN_NORMAL;
    _streams.meshTangent_id5 = PS_IN_TANGENT;
    _streams.PositionWS_id8 = PS_IN_POSITION_WS;
    _streams.DepthVS_id9 = PS_IN_DEPTH_VS;
    _streams.TexCoord_id49 = PS_IN_TEXCOORD0;
    _streams.ScreenPosition_id52 = PS_IN_SCREENPOSITION;
    _streams.IsFrontFace_id1 = PS_IN_SV_IsFrontFace;
    _streams.ScreenPosition_id52 /= vec4(_streams.ScreenPosition_id52.w);
    NormalBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

