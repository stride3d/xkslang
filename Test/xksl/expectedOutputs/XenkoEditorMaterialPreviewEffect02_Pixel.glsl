#version 450

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

struct IMaterialStreamBlend_Streams
{
    float matBlend;
    float matDisplacement;
    vec3 meshNormal;
    vec4 meshTangent;
    vec3 normalWS;
    mat3 tangentToWorld;
    vec4 Position;
    vec4 PositionWS;
    float DepthVS;
    vec3 matNormal;
    vec4 matColorBase;
    vec4 matDiffuse;
    float matGlossiness;
    vec3 matSpecular;
    float matSpecularIntensity;
    float matAmbientOcclusion;
    float matAmbientOcclusionDirectLightingFactor;
    float matCavity;
    float matCavityDiffuse;
    float matCavitySpecular;
    vec4 matEmissive;
    float matEmissiveIntensity;
    vec2 matDiffuseSpecularAlphaBlend;
    vec3 matAlphaBlendColor;
    float matAlphaDiscard;
    vec3 viewWS;
    vec3 matDiffuseVisible;
    float alphaRoughness;
    vec3 matSpecularVisible;
    float NdotV;
    vec3 lightPositionWS;
    vec3 lightDirectionWS;
    vec3 lightColor;
    vec3 lightColorNdotL;
    vec3 envLightDiffuseColor;
    vec3 envLightSpecularColor;
    float NdotL;
    float lightDirectAmbientOcclusion;
    int _unused;
};

struct MaterialSurfaceStreamsBlend_Streams
{
    float matBlend;
    int _unused;
};

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    bool IsFrontFace_id1;
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
    float matBlend_id43;
    vec3 shadowColor_id44;
    vec2 TexCoord_id45;
    vec2 TexCoord1_id46;
    vec2 TexCoord2_id47;
    vec2 TexCoord3_id48;
    vec2 TexCoord4_id49;
    vec2 TexCoord5_id50;
    vec2 TexCoord6_id51;
    vec2 TexCoord7_id52;
    vec2 TexCoord8_id53;
    vec2 TexCoord9_id54;
    uvec2 lightData_id55;
    int lightIndex_id56;
    vec4 ScreenPosition_id57;
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
    float o1S412C0_Camera_NearClipPlane;
    float o1S412C0_Camera_FarClipPlane;
    vec2 o1S412C0_Camera_ZProjection;
    vec2 o1S412C0_Camera_ViewSize;
    float o1S412C0_Camera_AspectRatio;
    vec4 o0S412C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S412C0_LightDirectionalGroup_Lights[8];
    int o0S412C0_DirectLightGroupPerView_LightCount;
    float o1S412C0_LightClustered_ClusterDepthScale;
    float o1S412C0_LightClustered_ClusterDepthBias;
    vec2 o1S412C0_LightClustered_ClusterStride;
    vec3 o3S397C0_LightSimpleAmbient_AmbientLight;
    vec4 o3S397C0_LightSimpleAmbient__padding_PerView_Lighting;
} PerView_var;

layout(std140) uniform PerMaterial
{
    vec2 o66S228C0_o58S2C0_o57S2C0_o56S2C0_o55S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o66S228C0_o58S2C0_o57S2C0_o56S2C0_o55S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o66S228C0_o58S2C0_o57S2C0_o54S2C0_o53S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o66S228C0_o58S2C0_o57S2C0_o52S2C0_o51S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o66S228C0_o58S2C0_o57S2C0_o50S2C0_o49S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o66S228C0_o58S2C0_o57S2C0_o48S2C0_o47S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o66S228C0_o58S2C0_o57S2C0_o46S2C0_o45S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o66S228C0_o58S2C0_o57S2C0_o46S2C0_o45S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o66S228C0_o58S2C0_o57S2C0_o44S2C0_o43S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec4 o66S228C0_o58S2C0_o57S2C0_o42S2C0_o41S2C0_ComputeColorConstantColorLink_constantColor;
    float o66S228C0_o58S2C0_o57S2C0_o40S2C0_o39S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o66S228C0_o58S2C0_o57S2C0_o38S2C0_o37S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o66S228C0_o58S2C0_o57S2C0_o38S2C0_o37S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    vec4 o66S228C0_o58S2C0_o57S2C0_o36S2C0_o35S2C0_ComputeColorConstantColorLink_constantColor;
    float o66S228C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o66S228C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o66S228C0_o19S2C0_o18S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o66S228C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o66S228C0_o15S2C0_o14S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o66S228C0_o15S2C0_o14S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o66S228C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec4 o66S228C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_constantColor;
    float o66S228C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o66S228C0_o7S2C0_o6S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o66S228C0_o7S2C0_o6S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    vec4 o66S228C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
} PerMaterial_var;

uniform samplerBuffer LightClusteredPointGroup_PointLights;
uniform usamplerBuffer LightClustered_LightIndices;
uniform samplerBuffer LightClusteredSpotGroup_SpotLights;
uniform sampler2D SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler;
uniform sampler2D SPIRV_Cross_CombinedMaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUTTexturing_LinearSampler;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_meshNormal;
layout(location = 2) in vec4 PS_IN_meshTangent;
layout(location = 3) in vec4 PS_IN_PositionWS;
layout(location = 4) in vec2 PS_IN_TexCoord;
layout(location = 5) in vec4 PS_IN_ScreenPosition;
layout(location = 6) in bool PS_IN_IsFrontFace;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void NormalUpdate_GenerateNormal_PS()
{
}

mat3 NormalUpdate_GetTangentMatrix(inout PS_STREAMS _streams)
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

void NormalUpdate_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    mat3 _95 = NormalUpdate_GetTangentMatrix(_streams);
    mat3 tangentMatrix = _95;
    mat3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id6 = tangentWorldTransform * tangentMatrix;
}

void NormalFromNormalMapping_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    NormalUpdate_GenerateNormal_PS();
    NormalUpdate_UpdateTangentToWorld(_streams);
}

void ShaderBase_PSMain()
{
}

void o67S228C1_IStreamInitializer_ResetStream()
{
}

void o67S228C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o67S228C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id43 = 0.0;
}

void o67S228C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o67S228C1_MaterialStream_ResetStream(_streams);
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
    _streams.matAlphaDiscard_id23 = 0.100000001490116119384765625;
}

void o67S228C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o67S228C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id30 = 1.0;
}

vec4 o66S228C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return PerMaterial_var.o66S228C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_constantColor;
}

void o66S228C0_o5S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o66S228C0_o5S2C0_o4S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id10 = colorBase;
    _streams.matColorBase_id9 = colorBase;
}

vec4 o66S228C0_o7S2C0_o6S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id45 * PerMaterial_var.o66S228C0_o7S2C0_o6S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o66S228C0_o7S2C0_o6S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o66S228C0_o7S2C0_MaterialSurfaceNormalMap_true_true__Compute(inout PS_STREAMS _streams)
{
    vec4 normal = o66S228C0_o7S2C0_o6S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0) - vec4(1.0);
    }
    if (true)
    {
        normal.z = sqrt(max(0.0, 1.0 - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id8 = normal.xyz;
}

vec4 o66S228C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o9S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o66S228C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0 - glossiness;
    }
    _streams.matGlossiness_id11 = glossiness;
}

vec4 o66S228C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return PerMaterial_var.o66S228C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_constantColor;
}

void o66S228C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecular_id12 = o66S228C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

vec4 o66S228C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o13S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id13 = o66S228C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

vec4 o66S228C0_o15S2C0_o14S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i1_Material_TextureOffset_i1__Compute(PS_STREAMS _streams)
{
    return vec4(texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id45 * PerMaterial_var.o66S228C0_o15S2C0_o14S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o66S228C0_o15S2C0_o14S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset).x);
}

void o66S228C0_o15S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusion_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusion_id14 = o66S228C0_o15S2C0_o14S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i1_Material_TextureOffset_i1__Compute(_streams).x;
}

vec4 o66S228C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o17S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusionDirectLightingFactor_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusionDirectLightingFactor_id15 = o66S228C0_o17S2C0_o16S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue__Compute().x;
}

vec4 o66S228C0_o19S2C0_o18S2C0_ComputeColorConstantFloatLink_Material_CavityValue__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o19S2C0_o18S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o19S2C0_o18S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o19S2C0_o18S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o19S2C0_o18S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o19S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavity_id16 = o66S228C0_o19S2C0_o18S2C0_ComputeColorConstantFloatLink_Material_CavityValue__Compute().x;
}

vec4 o66S228C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o21S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavityDiffuse_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavityDiffuse_id17 = o66S228C0_o21S2C0_o20S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue__Compute().x;
}

vec4 o66S228C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o23S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavitySpecular_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavitySpecular_id18 = o66S228C0_o23S2C0_o22S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue__Compute().x;
}

MaterialSurfaceStreamsBlend_Streams o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__getStreams(PS_STREAMS _streams)
{
    MaterialSurfaceStreamsBlend_Streams res = MaterialSurfaceStreamsBlend_Streams(_streams.matBlend_id43, 0);
    return res;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o36S2C0_o35S2C0_ComputeColorConstantColorLink_Material_DiffuseValue_i1__Compute()
{
    return PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o36S2C0_o35S2C0_ComputeColorConstantColorLink_constantColor;
}

void o66S228C0_o58S2C0_o57S2C0_o36S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o66S228C0_o58S2C0_o57S2C0_o36S2C0_o35S2C0_ComputeColorConstantColorLink_Material_DiffuseValue_i1__Compute();
    _streams.matDiffuse_id10 = colorBase;
    _streams.matColorBase_id9 = colorBase;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o38S2C0_o37S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_i1_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_i2_Material_TextureOffset_i2__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id45 * PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o38S2C0_o37S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o38S2C0_o37S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o66S228C0_o58S2C0_o57S2C0_o38S2C0_MaterialSurfaceNormalMap_true_true__Compute(inout PS_STREAMS _streams)
{
    vec4 normal = o66S228C0_o58S2C0_o57S2C0_o38S2C0_o37S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_NormalMap_i1_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_i2_Material_TextureOffset_i2__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0) - vec4(1.0);
    }
    if (true)
    {
        normal.z = sqrt(max(0.0, 1.0 - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id8 = normal.xyz;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o40S2C0_o39S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue_i1__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o40S2C0_o39S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o40S2C0_o39S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o40S2C0_o39S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o40S2C0_o39S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o58S2C0_o57S2C0_o40S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o66S228C0_o58S2C0_o57S2C0_o40S2C0_o39S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue_i1__Compute().x;
    if (false)
    {
        glossiness = 1.0 - glossiness;
    }
    _streams.matGlossiness_id11 = glossiness;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o42S2C0_o41S2C0_ComputeColorConstantColorLink_Material_SpecularValue_i1__Compute()
{
    return PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o42S2C0_o41S2C0_ComputeColorConstantColorLink_constantColor;
}

void o66S228C0_o58S2C0_o57S2C0_o42S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecular_id12 = o66S228C0_o58S2C0_o57S2C0_o42S2C0_o41S2C0_ComputeColorConstantColorLink_Material_SpecularValue_i1__Compute().xyz;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o44S2C0_o43S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue_i1__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o44S2C0_o43S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o44S2C0_o43S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o44S2C0_o43S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o44S2C0_o43S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o58S2C0_o57S2C0_o44S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id13 = o66S228C0_o58S2C0_o57S2C0_o44S2C0_o43S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue_i1__Compute().x;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o46S2C0_o45S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_i1_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i3_Material_TextureOffset_i3__Compute(PS_STREAMS _streams)
{
    return vec4(texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id45 * PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o46S2C0_o45S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o46S2C0_o45S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset).x);
}

void o66S228C0_o58S2C0_o57S2C0_o46S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusion_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusion_id14 = o66S228C0_o58S2C0_o57S2C0_o46S2C0_o45S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_AmbientOcclusionMap_i1_TEXCOORD0_Material_Sampler_i0_r_Material_TextureScale_i3_Material_TextureOffset_i3__Compute(_streams).x;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o48S2C0_o47S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue_i1__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o48S2C0_o47S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o48S2C0_o47S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o48S2C0_o47S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o48S2C0_o47S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o58S2C0_o57S2C0_o48S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusionDirectLightingFactor_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matAmbientOcclusionDirectLightingFactor_id15 = o66S228C0_o58S2C0_o57S2C0_o48S2C0_o47S2C0_ComputeColorConstantFloatLink_Material_AmbientOcclusionDirectLightingFactorValue_i1__Compute().x;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o50S2C0_o49S2C0_ComputeColorConstantFloatLink_Material_CavityValue_i1__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o50S2C0_o49S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o50S2C0_o49S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o50S2C0_o49S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o50S2C0_o49S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o58S2C0_o57S2C0_o50S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavity_id16 = o66S228C0_o58S2C0_o57S2C0_o50S2C0_o49S2C0_ComputeColorConstantFloatLink_Material_CavityValue_i1__Compute().x;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o52S2C0_o51S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue_i1__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o52S2C0_o51S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o52S2C0_o51S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o52S2C0_o51S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o52S2C0_o51S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o58S2C0_o57S2C0_o52S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavityDiffuse_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavityDiffuse_id17 = o66S228C0_o58S2C0_o57S2C0_o52S2C0_o51S2C0_ComputeColorConstantFloatLink_Material_CavityDiffuseValue_i1__Compute().x;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o54S2C0_o53S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue_i1__Compute()
{
    return vec4(PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o54S2C0_o53S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o54S2C0_o53S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o54S2C0_o53S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o54S2C0_o53S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o66S228C0_o58S2C0_o57S2C0_o54S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavitySpecular_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matCavitySpecular_id18 = o66S228C0_o58S2C0_o57S2C0_o54S2C0_o53S2C0_ComputeColorConstantFloatLink_Material_CavitySpecularValue_i1__Compute().x;
}

vec4 o66S228C0_o58S2C0_o57S2C0_o56S2C0_o55S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_BlendMap_TEXCOORD0_Material_Sampler_i0_a_Material_TextureScale_i4_Material_TextureOffset_i4__Compute(PS_STREAMS _streams)
{
    return vec4(texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id45 * PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o56S2C0_o55S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o66S228C0_o58S2C0_o57S2C0_o56S2C0_o55S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset).w);
}

void o66S228C0_o58S2C0_o57S2C0_o56S2C0_MaterialSurfaceSetStreamFromComputeColor_matBlend_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matBlend_id43 = o66S228C0_o58S2C0_o57S2C0_o56S2C0_o55S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_BlendMap_TEXCOORD0_Material_Sampler_i0_a_Material_TextureScale_i4_Material_TextureOffset_i4__Compute(_streams).x;
}

void o66S228C0_o58S2C0_o57S2C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o66S228C0_o58S2C0_o57S2C0_o36S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o38S2C0_MaterialSurfaceNormalMap_true_true__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o40S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o42S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o44S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o46S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusion_r__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o48S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusionDirectLightingFactor_r__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o50S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavity_r__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o52S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavityDiffuse_r__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o54S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavitySpecular_r__Compute(_streams);
    o66S228C0_o58S2C0_o57S2C0_o56S2C0_MaterialSurfaceSetStreamFromComputeColor_matBlend_r__Compute(_streams);
}

IMaterialStreamBlend_Streams o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(MaterialSurfaceStreamsBlend_Streams s)
{
    IMaterialStreamBlend_Streams r = IMaterialStreamBlend_Streams(s.matBlend, 0.0, vec3(0.0), vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, s._unused);
    return r;
}

void o66S228C0_o58S2C0_o24S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o25S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o26S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o27S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o28S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o29S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o30S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o31S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o32S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o33S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_o34S2C1_IMaterialStreamBlend_Compute(IMaterialStreamBlend_Streams fromStream)
{
}

void o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend_Compute(inout PS_STREAMS _streams)
{
    MaterialSurfaceStreamsBlend_Streams backup = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__getStreams(_streams);
    o66S228C0_o58S2C0_o57S2C0_MaterialSurfaceArray_Compute(_streams);
    MaterialSurfaceStreamsBlend_Streams param = backup;
    IMaterialStreamBlend_Streams param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o24S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o25S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o26S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o27S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o28S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o29S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o30S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o31S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o32S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o33S2C1_IMaterialStreamBlend_Compute(param_1);
    param = backup;
    param_1 = o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend__ConvertMaterialSurfaceStreamsBlendStreamsToIMaterialStreamBlendStreams(param);
    o66S228C0_o58S2C0_o34S2C1_IMaterialStreamBlend_Compute(param_1);
}

void NormalUpdate_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id5 = normalize(_streams.tangentToWorld_id6 * normalInTangentSpace);
}

void o66S228C0_o65S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
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

void o66S228C0_o65S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id42 = mix(1.0, _streams.matAmbientOcclusion_id14, _streams.matAmbientOcclusionDirectLightingFactor_id15);
    _streams.matDiffuseVisible_id25 = ((_streams.matDiffuse_id10.xyz * mix(1.0, _streams.matCavity_id16, _streams.matCavityDiffuse_id17)) * _streams.matDiffuseSpecularAlphaBlend_id21.x) * _streams.matAlphaBlendColor_id22;
    _streams.matSpecularVisible_id27 = (((_streams.matSpecular_id12 * _streams.matSpecularIntensity_id13) * mix(1.0, _streams.matCavity_id16, _streams.matCavitySpecular_id18)) * _streams.matDiffuseSpecularAlphaBlend_id21.y) * _streams.matAlphaBlendColor_id22;
    _streams.NdotV_id28 = max(dot(_streams.normalWS_id5, _streams.viewWS_id24), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id11;
    _streams.alphaRoughness_id26 = max(roughness * roughness, 0.001000000047497451305389404296875);
}

void o0S412C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S412C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S412C0_DirectLightGroupPerView_GetLightCount()
{
    return PerView_var.o0S412C0_DirectLightGroupPerView_LightCount;
}

void o0S412C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id37 = PerView_var.o0S412C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id36 = -PerView_var.o0S412C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

vec3 o0S412C0_ShadowGroup_ComputeShadow(vec3 position, int lightIndex)
{
    return vec3(1.0);
}

void o0S412C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S412C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id5, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id7.xyz;
    int param_2 = lightIndex;
    _streams.shadowColor_id44 = o0S412C0_ShadowGroup_ComputeShadow(param_1, param_2);
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id44) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id31 = normalize(_streams.viewWS_id24 + _streams.lightDirectionWS_id36);
    _streams.NdotH_id32 = clamp(dot(_streams.normalWS_id5, _streams.H_id31), 0.0, 1.0);
    _streams.LdotH_id33 = clamp(dot(_streams.lightDirectionWS_id36, _streams.H_id31), 0.0, 1.0);
    _streams.VdotH_id34 = _streams.LdotH_id33;
}

vec3 o66S228C0_o65S2C0_o59S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (true)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id27);
    }
    return ((diffuseColor / vec3(3.1415927410125732421875)) * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.x;
}

vec3 o66S228C0_o65S2C0_o64S2C0_o61S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, vec3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0 - lOrVDotH, 5.0));
}

vec3 o66S228C0_o65S2C0_o64S2C0_o61S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, float lOrVDotH)
{
    vec3 param = f0;
    vec3 param_1 = vec3(1.0);
    float param_2 = lOrVDotH;
    return o66S228C0_o65S2C0_o64S2C0_o61S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

vec3 o66S228C0_o65S2C0_o64S2C0_o61S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, vec3 f0)
{
    vec3 param = f0;
    float param_1 = _streams.LdotH_id33;
    return o66S228C0_o65S2C0_o64S2C0_o61S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o66S228C0_o65S2C0_o64S2C0_o62S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5;
    return nDotX / ((nDotX * (1.0 - k)) + k);
}

float o66S228C0_o65S2C0_o64S2C0_o62S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(float alphaR, float nDotL, float nDotV)
{
    float param = alphaR;
    float param_1 = nDotL;
    float param_2 = alphaR;
    float param_3 = nDotV;
    return (o66S228C0_o65S2C0_o64S2C0_o62S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param, param_1) * o66S228C0_o65S2C0_o64S2C0_o62S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param_2, param_3)) / (nDotL * nDotV);
}

float o66S228C0_o65S2C0_o64S2C0_o62S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id26;
    float param_1 = _streams.NdotL_id41;
    float param_2 = _streams.NdotV_id28;
    return o66S228C0_o65S2C0_o64S2C0_o62S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(param, param_1, param_2);
}

float o66S228C0_o65S2C0_o64S2C0_o63S2C2_BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0)) + 1.0, 9.9999997473787516355514526367188e-05);
    return alphaR2 / ((3.1415927410125732421875 * d) * d);
}

float o66S228C0_o65S2C0_o64S2C0_o63S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id26;
    float param_1 = _streams.NdotH_id32;
    return o66S228C0_o65S2C0_o64S2C0_o63S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

vec3 o66S228C0_o65S2C0_o64S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id27;
    vec3 param = specularColor;
    vec3 fresnel = o66S228C0_o65S2C0_o64S2C0_o61S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o66S228C0_o65S2C0_o64S2C0_o62S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o66S228C0_o65S2C0_o64S2C0_o63S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    vec3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / vec3(4.0);
    return (reflected * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.y;
}

void o1S412C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = PerView_var.o1S412C0_Camera_ZProjection.y / (projectedDepth - PerView_var.o1S412C0_Camera_ZProjection.x);
    vec2 texCoord = vec2(_streams.ScreenPosition_id57.x + 1.0, 1.0 - _streams.ScreenPosition_id57.y) * 0.5;
    int slice = int(max(log2((depth * PerView_var.o1S412C0_LightClustered_ClusterDepthScale) + PerView_var.o1S412C0_LightClustered_ClusterDepthBias), 0.0));
    _streams.lightData_id55 = uvec2(texelFetch(LightClustered_LightClusters, ivec4(ivec2(texCoord * PerView_var.o1S412C0_LightClustered_ClusterStride), slice, 0).xyz, ivec4(ivec2(texCoord * PerView_var.o1S412C0_LightClustered_ClusterStride), slice, 0).w).xy);
    _streams.lightIndex_id56 = int(_streams.lightData_id55.x);
}

void o1S412C0_LightClusteredPointGroup_PrepareDirectLights(inout PS_STREAMS _streams)
{
    o1S412C0_LightClustered_PrepareLightData(_streams);
}

int o1S412C0_LightClusteredPointGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id55.y & 65535u);
}

int o1S412C0_LightClusteredPointGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id55.y & 65535u);
}

float o1S412C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o1S412C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S412C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S412C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, vec3 position, out vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    float lightInvSquareRadius = light.InvSquareRadius;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S412C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S412C0_LightPoint_ProcessLight(inout PS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id7.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _393 = o1S412C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _393;
    _streams.lightPositionWS_id35 = light.PositionWS;
    _streams.lightColor_id37 = light.Color * attenuation;
    _streams.lightDirectionWS_id36 = lightVectorNorm;
}

void o1S412C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id56).x);
    _streams.lightIndex_id56++;
    vec4 pointLight1 = texelFetch(LightClusteredPointGroup_PointLights, realLightIndex * 2);
    vec4 pointLight2 = texelFetch(LightClusteredPointGroup_PointLights, (realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S412C0_LightPoint_ProcessLight(_streams, param);
}

vec3 o1S412C0_ShadowGroup_ComputeShadow(vec3 position, int lightIndex)
{
    return vec3(1.0);
}

void o1S412C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S412C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id5, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id7.xyz;
    int param_2 = lightIndex;
    _streams.shadowColor_id44 = o1S412C0_ShadowGroup_ComputeShadow(param_1, param_2);
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id44) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void o2S412C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S412C0_LightClusteredSpotGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id55.y >> uint(16));
}

int o2S412C0_LightClusteredSpotGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id55.y >> uint(16));
}

float o2S412C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o2S412C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S412C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S412C0_LightUtil_GetAngleAttenuation(vec3 lightVector, vec3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0, 1.0);
    attenuation *= attenuation;
    return attenuation;
}

float o2S412C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, vec3 position, inout vec3 lightVectorNorm)
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
    attenuation *= o2S412C0_LightUtil_GetDistanceAttenuation(param, param_1);
    vec3 param_2 = lightVectorNorm;
    vec3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S412C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S412C0_LightSpot_ProcessLight(inout PS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id7.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _679 = o2S412C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _679;
    _streams.lightColor_id37 = light.Color * attenuation;
    _streams.lightDirectionWS_id36 = lightVectorNorm;
}

void o2S412C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id56).x);
    _streams.lightIndex_id56++;
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
    o2S412C0_LightSpot_ProcessLight(_streams, param);
}

vec3 o2S412C0_ShadowGroup_ComputeShadow(vec3 position, int lightIndex)
{
    return vec3(1.0);
}

void o2S412C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S412C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id5, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id7.xyz;
    int param_2 = lightIndex;
    _streams.shadowColor_id44 = o2S412C0_ShadowGroup_ComputeShadow(param_1, param_2);
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id44) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void o3S397C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id39 = vec3(0.0);
    _streams.envLightSpecularColor_id40 = vec3(0.0);
}

void o3S397C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S397C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    vec3 lightColor = PerView_var.o3S397C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id14;
    _streams.envLightDiffuseColor_id39 = lightColor;
    _streams.envLightSpecularColor_id40 = lightColor;
}

vec3 o66S228C0_o65S2C0_o59S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (true)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id27);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id39;
}

vec3 o66S228C0_o65S2C0_o64S2C0_o60S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(vec3 specularColor, float alphaR, float nDotV)
{
    float glossiness = 1.0 - sqrt(alphaR);
    vec4 environmentLightingDFG = textureLod(SPIRV_Cross_CombinedMaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUTTexturing_LinearSampler, vec2(glossiness, nDotV), 0.0);
    return (specularColor * environmentLightingDFG.x) + vec3(environmentLightingDFG.y);
}

vec3 o66S228C0_o65S2C0_o64S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id27;
    vec3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id26;
    float param_2 = _streams.NdotV_id28;
    return o66S228C0_o65S2C0_o64S2C0_o60S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(param, param_1, param_2) * _streams.envLightSpecularColor_id40;
}

void o66S228C0_o65S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id8;
    NormalUpdate_UpdateNormalFromTangentSpace(_streams, param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id5 = -_streams.normalWS_id5;
    }
    o66S228C0_o65S2C0_LightStream_ResetLightStream(_streams);
    o66S228C0_o65S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    vec3 directLightingContribution = vec3(0.0);
    o0S412C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S412C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S412C0_DirectLightGroupPerView_GetLightCount();
    int i = 0;
    int param_1;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o0S412C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o66S228C0_o65S2C0_o59S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o66S228C0_o65S2C0_o64S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    o1S412C0_LightClusteredPointGroup_PrepareDirectLights(_streams);
    maxLightCount = o1S412C0_LightClusteredPointGroup_GetMaxLightCount(_streams);
    count = o1S412C0_LightClusteredPointGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o1S412C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o66S228C0_o65S2C0_o59S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o66S228C0_o65S2C0_o64S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    o2S412C0_DirectLightGroup_PrepareDirectLights();
    maxLightCount = o2S412C0_LightClusteredSpotGroup_GetMaxLightCount(_streams);
    count = o2S412C0_LightClusteredSpotGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o2S412C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o66S228C0_o65S2C0_o59S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o66S228C0_o65S2C0_o64S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    vec3 environmentLightingContribution = vec3(0.0);
    o3S397C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o66S228C0_o65S2C0_o59S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o66S228C0_o65S2C0_o64S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id29 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id30 = _streams.matDiffuse_id10.w;
}

void o66S228C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o66S228C0_o5S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o66S228C0_o7S2C0_MaterialSurfaceNormalMap_true_true__Compute(_streams);
    o66S228C0_o9S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o66S228C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o66S228C0_o13S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o66S228C0_o15S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusion_r__Compute(_streams);
    o66S228C0_o17S2C0_MaterialSurfaceSetStreamFromComputeColor_matAmbientOcclusionDirectLightingFactor_r__Compute(_streams);
    o66S228C0_o19S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavity_r__Compute(_streams);
    o66S228C0_o21S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavityDiffuse_r__Compute(_streams);
    o66S228C0_o23S2C0_MaterialSurfaceSetStreamFromComputeColor_matCavitySpecular_r__Compute(_streams);
    o66S228C0_o58S2C0_MaterialSurfaceStreamsBlend_Compute(_streams);
    o66S228C0_o65S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id24 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id7.xyz);
    _streams.shadingColor_id29 = vec3(0.0);
    o67S228C1_MaterialPixelShadingStream_ResetStream(_streams);
    o66S228C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id29, _streams.shadingColorAlpha_id30);
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

void SharedTextureCoordinate_PSMain(inout PS_STREAMS _streams)
{
    _streams.TexCoord1_id46 = _streams.TexCoord_id45;
    _streams.TexCoord2_id47 = _streams.TexCoord_id45;
    _streams.TexCoord3_id48 = _streams.TexCoord_id45;
    _streams.TexCoord4_id49 = _streams.TexCoord_id45;
    _streams.TexCoord5_id50 = _streams.TexCoord_id45;
    _streams.TexCoord6_id51 = _streams.TexCoord_id45;
    _streams.TexCoord7_id52 = _streams.TexCoord_id45;
    _streams.TexCoord8_id53 = _streams.TexCoord_id45;
    _streams.TexCoord9_id54 = _streams.TexCoord_id45;
    NormalBase_PSMain(_streams);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), false, vec4(0.0), vec3(0.0), vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), uvec2(0u), 0, vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.meshNormal_id3 = PS_IN_meshNormal;
    _streams.meshTangent_id4 = PS_IN_meshTangent;
    _streams.PositionWS_id7 = PS_IN_PositionWS;
    _streams.TexCoord_id45 = PS_IN_TexCoord;
    _streams.ScreenPosition_id57 = PS_IN_ScreenPosition;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    _streams.ScreenPosition_id57 /= vec4(_streams.ScreenPosition_id57.w);
    SharedTextureCoordinate_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

