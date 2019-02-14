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
    float matBlend_id3;
    vec3 meshNormal_id4;
    vec3 meshNormalWS_id5;
    vec4 meshTangent_id6;
    vec3 normalWS_id7;
    mat3 tangentToWorld_id8;
    vec4 PositionWS_id9;
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
    vec3 shadowColor_id46;
    float thicknessWS_id47;
    vec2 TexCoord_id48;
    vec2 TexCoord1_id49;
    vec2 TexCoord2_id50;
    vec2 TexCoord3_id51;
    vec2 TexCoord4_id52;
    vec2 TexCoord5_id53;
    vec2 TexCoord6_id54;
    vec2 TexCoord7_id55;
    vec2 TexCoord8_id56;
    vec2 TexCoord9_id57;
    uvec2 lightData_id58;
    int lightIndex_id59;
    vec4 ScreenPosition_id60;
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
    float o1S439C0_Camera_NearClipPlane;
    float o1S439C0_Camera_FarClipPlane;
    vec2 o1S439C0_Camera_ZProjection;
    vec2 o1S439C0_Camera_ViewSize;
    float o1S439C0_Camera_AspectRatio;
    vec4 o0S439C0_LightDirectionalGroup_padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S439C0_LightDirectionalGroup_Lights[8];
    int o0S439C0_DirectLightGroupPerView_LightCount;
    float o1S439C0_LightClustered_ClusterDepthScale;
    float o1S439C0_LightClustered_ClusterDepthBias;
    vec2 o1S439C0_LightClustered_ClusterStride;
    vec3 o3S423C0_LightSimpleAmbient_AmbientLight;
    vec4 o3S423C0_LightSimpleAmbient_padding_PerView_Lighting;
} PerView_var;

layout(std140) uniform PerMaterial
{
    float o21S267C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec4 o21S267C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_constantColor;
    float o21S267C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o21S267C0_o5S2C0_o4S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o21S267C0_o5S2C0_o4S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o27S35C0_o25S2C0_o24S2C0_o23S2C1_ComputeColorConstantFloatLink_constantFloat;
} PerMaterial_var;

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

uniform samplerBuffer LightClusteredPointGroup_PointLights;
uniform usamplerBuffer LightClustered_LightIndices;
uniform samplerBuffer LightClusteredSpotGroup_SpotLights;
uniform sampler2D SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler;
uniform usampler3D SPIRV_Cross_CombinedLightClustered_LightClustersSPIRV_Cross_DummySampler;
uniform sampler2D SPIRV_Cross_CombinedMaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUTTexturing_LinearSampler;

in vec4 PS_IN_SV_Position;
in vec3 PS_IN_NORMAL;
in vec4 PS_IN_TANGENT;
in vec4 PS_IN_POSITION_WS;
in vec2 PS_IN_TEXCOORD0;
in vec4 PS_IN_SCREENPOSITION;
in bool PS_IN_SV_IsFrontFace;
out vec4 PS_OUT_ColorTarget;

void NormalUpdate_GenerateNormal_PS()
{
}

mat3 NormalUpdate_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id4 = normalize(_streams.meshNormal_id4);
    vec3 tangent = normalize(_streams.meshTangent_id6.xyz);
    vec3 bitangent = cross(_streams.meshNormal_id4, tangent) * _streams.meshTangent_id6.w;
    mat3 tangentMatrix = mat3(vec3(tangent), vec3(bitangent), vec3(_streams.meshNormal_id4));
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
    _streams.tangentToWorld_id8 = tangentWorldTransform * tangentMatrix;
}

void NormalFromNormalMapping_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    NormalUpdate_GenerateNormal_PS();
    NormalUpdate_UpdateTangentToWorld(_streams);
    _streams.meshNormalWS_id5 = mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z)) * _streams.meshNormal_id4;
}

void ShaderBase_PSMain()
{
}

void o28S267C1_IStreamInitializer_ResetStream()
{
}

void o28S267C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o28S267C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id3 = 0.0;
}

void o28S267C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o28S267C1_MaterialStream_ResetStream(_streams);
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

void o28S267C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o28S267C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id33 = 1.0;
}

vec4 o21S267C0_o5S2C0_o4S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id48 * PerMaterial_var.o21S267C0_o5S2C0_o4S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o21S267C0_o5S2C0_o4S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o21S267C0_o5S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o21S267C0_o5S2C0_o4S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id12 = colorBase;
    _streams.matColorBase_id11 = colorBase;
}

vec2 o21S267C0_o7S2C0_o6S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    return vec2(x, 1.0) / vec2(sqrt(1.0 + (x * x)));
}

vec4 o21S267C0_o7S2C0_o6S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    vec2 offset = _streams.TexCoord_id48 - vec2(0.5);
    float phase = length(offset);
    float derivative = cos((((phase + (PerFrame_var.Global_Time * (-0.02999999932944774627685546875))) * 2.0) * 3.1400001049041748046875) * 5.0) * 0.100000001490116119384765625;
    float param = offset.y / offset.x;
    vec2 xz = o21S267C0_o7S2C0_o6S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    vec2 xy = o21S267C0_o7S2C0_o6S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    vec2 _1296 = (((xz.yx * sign(offset.x)) * (-xy.x)) * 0.5) + vec2(0.5);
    vec3 normal;
    normal = vec3(_1296.x, _1296.y, normal.z);
    normal.z = xy.y;
    return vec4(normal, 1.0);
}

void o21S267C0_o7S2C0_MaterialSurfaceNormalMap_false_true__Compute(inout PS_STREAMS _streams)
{
    vec4 normal = o21S267C0_o7S2C0_o6S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0) - vec4(1.0);
    }
    if (false)
    {
        normal.z = sqrt(max(0.0, 1.0 - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id10 = normal.xyz;
}

vec4 o21S267C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return vec4(PerMaterial_var.o21S267C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o21S267C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o21S267C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o21S267C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o21S267C0_o9S2C0_MaterialSurfaceGlossinessMap_false__Compute(inout PS_STREAMS _streams)
{
    float glossiness = o21S267C0_o9S2C0_o8S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0 - glossiness;
    }
    _streams.matGlossiness_id13 = glossiness;
}

vec4 o21S267C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return PerMaterial_var.o21S267C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_constantColor;
}

void o21S267C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecular_id14 = o21S267C0_o11S2C0_o10S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

vec4 o21S267C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return vec4(PerMaterial_var.o21S267C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o21S267C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o21S267C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o21S267C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o21S267C0_o13S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id15 = o21S267C0_o13S2C0_o12S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void NormalUpdate_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id7 = normalize(_streams.tangentToWorld_id8 * normalInTangentSpace);
}

void o21S267C0_o20S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
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

void o21S267C0_o20S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id45 = mix(1.0, _streams.matAmbientOcclusion_id16, _streams.matAmbientOcclusionDirectLightingFactor_id17);
    _streams.matDiffuseVisible_id28 = ((_streams.matDiffuse_id12.xyz * mix(1.0, _streams.matCavity_id18, _streams.matCavityDiffuse_id19)) * _streams.matDiffuseSpecularAlphaBlend_id24.x) * _streams.matAlphaBlendColor_id25;
    _streams.matSpecularVisible_id30 = (((_streams.matSpecular_id14 * _streams.matSpecularIntensity_id15) * mix(1.0, _streams.matCavity_id18, _streams.matCavitySpecular_id20)) * _streams.matDiffuseSpecularAlphaBlend_id24.y) * _streams.matAlphaBlendColor_id25;
    _streams.NdotV_id31 = max(dot(_streams.normalWS_id7, _streams.viewWS_id27), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id13;
    _streams.alphaRoughness_id29 = max(roughness * roughness, 0.001000000047497451305389404296875);
}

void o21S267C0_o20S2C0_o14S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o21S267C0_o20S2C0_o19S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
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
    return PerView_var.o0S439C0_DirectLightGroupPerView_LightCount;
}

void o0S439C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id40 = PerView_var.o0S439C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id39 = -PerView_var.o0S439C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

vec3 o0S439C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id47 = 0.0;
    return vec3(1.0);
}

void o0S439C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S439C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id7, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    vec3 _269 = o0S439C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id46 = _269;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id46) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id34 = normalize(_streams.viewWS_id27 + _streams.lightDirectionWS_id39);
    _streams.NdotH_id35 = clamp(dot(_streams.normalWS_id7, _streams.H_id34), 0.0, 1.0);
    _streams.LdotH_id36 = clamp(dot(_streams.lightDirectionWS_id39, _streams.H_id34), 0.0, 1.0);
    _streams.VdotH_id37 = _streams.LdotH_id36;
}

vec3 o21S267C0_o20S2C0_o14S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id30);
    }
    return ((diffuseColor / vec3(3.1415927410125732421875)) * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.x;
}

vec3 o21S267C0_o20S2C0_o19S2C0_o16S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, vec3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0 - lOrVDotH, 5.0));
}

vec3 o21S267C0_o20S2C0_o19S2C0_o16S2C0_BRDFMicrofacet_FresnelSchlick(vec3 f0, float lOrVDotH)
{
    vec3 param = f0;
    vec3 param_1 = vec3(1.0);
    float param_2 = lOrVDotH;
    return o21S267C0_o20S2C0_o19S2C0_o16S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

vec3 o21S267C0_o20S2C0_o19S2C0_o16S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, vec3 f0)
{
    vec3 param = f0;
    float param_1 = _streams.LdotH_id36;
    return o21S267C0_o20S2C0_o19S2C0_o16S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o21S267C0_o20S2C0_o19S2C0_o17S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5;
    return nDotX / ((nDotX * (1.0 - k)) + k);
}

float o21S267C0_o20S2C0_o19S2C0_o17S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(float alphaR, float nDotL, float nDotV)
{
    float param = alphaR;
    float param_1 = nDotL;
    float param_2 = alphaR;
    float param_3 = nDotV;
    return (o21S267C0_o20S2C0_o19S2C0_o17S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param, param_1) * o21S267C0_o20S2C0_o19S2C0_o17S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(param_2, param_3)) / (nDotL * nDotV);
}

float o21S267C0_o20S2C0_o19S2C0_o17S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id29;
    float param_1 = _streams.NdotL_id44;
    float param_2 = _streams.NdotV_id31;
    return o21S267C0_o20S2C0_o19S2C0_o17S2C1_BRDFMicrofacet_VisibilitySmithSchlickGGX(param, param_1, param_2);
}

float o21S267C0_o20S2C0_o19S2C0_o18S2C2_BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0)) + 1.0, 9.9999997473787516355514526367188e-05);
    return alphaR2 / ((3.1415927410125732421875 * d) * d);
}

float o21S267C0_o20S2C0_o19S2C0_o18S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id29;
    float param_1 = _streams.NdotH_id35;
    return o21S267C0_o20S2C0_o19S2C0_o18S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

vec3 o21S267C0_o20S2C0_o19S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id30;
    vec3 param = specularColor;
    vec3 fresnel = o21S267C0_o20S2C0_o19S2C0_o16S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o21S267C0_o20S2C0_o19S2C0_o17S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o21S267C0_o20S2C0_o19S2C0_o18S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    vec3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / vec3(4.0);
    return (reflected * _streams.lightColorNdotL_id41) * _streams.matDiffuseSpecularAlphaBlend_id24.y;
}

void o1S439C0_LightClustered_PrepareLightData(inout PS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id0.z;
    float depth = PerView_var.o1S439C0_Camera_ZProjection.y / (projectedDepth - PerView_var.o1S439C0_Camera_ZProjection.x);
    vec2 texCoord = vec2(_streams.ScreenPosition_id60.x + 1.0, 1.0 - _streams.ScreenPosition_id60.y) * 0.5;
    int slice = int(max(log2((depth * PerView_var.o1S439C0_LightClustered_ClusterDepthScale) + PerView_var.o1S439C0_LightClustered_ClusterDepthBias), 0.0));
    _streams.lightData_id58 = uvec2(texelFetch(SPIRV_Cross_CombinedLightClustered_LightClustersSPIRV_Cross_DummySampler, ivec4(ivec2(texCoord * PerView_var.o1S439C0_LightClustered_ClusterStride), slice, 0).xyz, ivec4(ivec2(texCoord * PerView_var.o1S439C0_LightClustered_ClusterStride), slice, 0).w).xy);
    _streams.lightIndex_id59 = int(_streams.lightData_id58.x);
}

void o1S439C0_LightClusteredPointGroup_PrepareDirectLights(inout PS_STREAMS _streams)
{
    o1S439C0_LightClustered_PrepareLightData(_streams);
}

int o1S439C0_LightClusteredPointGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id58.y & 65535u);
}

int o1S439C0_LightClusteredPointGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id58.y & 65535u);
}

float o1S439C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o1S439C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S439C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S439C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, vec3 position, out vec3 lightVectorNorm)
{
    vec3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / vec3(lightVectorLength);
    float lightInvSquareRadius = light.InvSquareRadius;
    vec3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S439C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S439C0_LightPoint_ProcessLight(inout PS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _414 = o1S439C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _414;
    _streams.lightPositionWS_id38 = light.PositionWS;
    _streams.lightColor_id40 = light.Color * attenuation;
    _streams.lightDirectionWS_id39 = lightVectorNorm;
}

void o1S439C0_LightClusteredPointGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id59).x);
    _streams.lightIndex_id59++;
    vec4 pointLight1 = texelFetch(LightClusteredPointGroup_PointLights, realLightIndex * 2);
    vec4 pointLight2 = texelFetch(LightClusteredPointGroup_PointLights, (realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S439C0_LightPoint_ProcessLight(_streams, param);
}

vec3 o1S439C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id47 = 0.0;
    return vec3(1.0);
}

void o1S439C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S439C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id7, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    vec3 _330 = o1S439C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id46 = _330;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id46) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void o2S439C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S439C0_LightClusteredSpotGroup_GetMaxLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id58.y >> uint(16));
}

int o2S439C0_LightClusteredSpotGroup_GetLightCount(PS_STREAMS _streams)
{
    return int(_streams.lightData_id58.y >> uint(16));
}

float o2S439C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0 - (factor * factor), 0.0, 1.0);
    return smoothFactor * smoothFactor;
}

float o2S439C0_LightUtil_GetDistanceAttenuation(vec3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0 / max(d2, 9.9999997473787516355514526367188e-05);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S439C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S439C0_LightUtil_GetAngleAttenuation(vec3 lightVector, vec3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0, 1.0);
    attenuation *= attenuation;
    return attenuation;
}

float o2S439C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, vec3 position, inout vec3 lightVectorNorm)
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
    attenuation *= o2S439C0_LightUtil_GetDistanceAttenuation(param, param_1);
    vec3 param_2 = lightVectorNorm;
    vec3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S439C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S439C0_LightSpot_ProcessLight(inout PS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    vec3 lightVectorNorm;
    vec3 param_2 = lightVectorNorm;
    float _701 = o2S439C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _701;
    _streams.lightColor_id40 = light.Color * attenuation;
    _streams.lightDirectionWS_id39 = lightVectorNorm;
}

void o2S439C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(LightClustered_LightIndices, _streams.lightIndex_id59).x);
    _streams.lightIndex_id59++;
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
    o2S439C0_LightSpot_ProcessLight(_streams, param);
}

vec3 o2S439C0_ShadowGroup_ComputeShadow(inout PS_STREAMS _streams, vec3 position, int lightIndex)
{
    _streams.thicknessWS_id47 = 0.0;
    return vec3(1.0);
}

void o2S439C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S439C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id44 = max(dot(_streams.normalWS_id7, _streams.lightDirectionWS_id39), 9.9999997473787516355514526367188e-05);
    vec3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    vec3 _605 = o2S439C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id46 = _605;
    _streams.lightColorNdotL_id41 = ((_streams.lightColor_id40 * _streams.shadowColor_id46) * _streams.NdotL_id44) * _streams.lightDirectAmbientOcclusion_id45;
}

void o3S423C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id42 = vec3(0.0);
    _streams.envLightSpecularColor_id43 = vec3(0.0);
}

void o3S423C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o3S423C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    vec3 lightColor = PerView_var.o3S423C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id16;
    _streams.envLightDiffuseColor_id42 = lightColor;
    _streams.envLightSpecularColor_id43 = lightColor;
}

vec3 o21S267C0_o20S2C0_o14S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id28;
    if (true)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id30);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id42;
}

vec3 o21S267C0_o20S2C0_o19S2C0_o15S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(vec3 specularColor, float alphaR, float nDotV)
{
    float glossiness = 1.0 - sqrt(alphaR);
    vec4 environmentLightingDFG = textureLod(SPIRV_Cross_CombinedMaterialSpecularMicrofacetEnvironmentGGXLUT_EnvironmentLightingDFG_LUTTexturing_LinearSampler, vec2(glossiness, nDotV), 0.0);
    return (specularColor * environmentLightingDFG.x) + vec3(environmentLightingDFG.y);
}

vec3 o21S267C0_o20S2C0_o19S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 specularColor = _streams.matSpecularVisible_id30;
    vec3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id29;
    float param_2 = _streams.NdotV_id31;
    return o21S267C0_o20S2C0_o19S2C0_o15S2C3_MaterialSpecularMicrofacetEnvironmentGGXLUT_Compute(param, param_1, param_2) * _streams.envLightSpecularColor_id43;
}

void o21S267C0_o20S2C0_o14S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o21S267C0_o20S2C0_o19S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o21S267C0_o20S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id10;
    NormalUpdate_UpdateNormalFromTangentSpace(_streams, param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id7 = -_streams.normalWS_id7;
    }
    o21S267C0_o20S2C0_LightStream_ResetLightStream(_streams);
    o21S267C0_o20S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o21S267C0_o20S2C0_o14S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    o21S267C0_o20S2C0_o19S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    vec3 directLightingContribution = vec3(0.0);
    o0S439C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S439C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S439C0_DirectLightGroupPerView_GetLightCount();
    int i = 0;
    int param_1;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o0S439C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o21S267C0_o20S2C0_o14S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o21S267C0_o20S2C0_o19S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    o1S439C0_LightClusteredPointGroup_PrepareDirectLights(_streams);
    maxLightCount = o1S439C0_LightClusteredPointGroup_GetMaxLightCount(_streams);
    count = o1S439C0_LightClusteredPointGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o1S439C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o21S267C0_o20S2C0_o14S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o21S267C0_o20S2C0_o19S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    o2S439C0_DirectLightGroup_PrepareDirectLights();
    maxLightCount = o2S439C0_LightClusteredSpotGroup_GetMaxLightCount(_streams);
    count = o2S439C0_LightClusteredSpotGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param_1 = i;
        o2S439C0_DirectLightGroup_PrepareDirectLight(_streams, param_1);
        MaterialPixelShadingStream_PrepareMaterialPerDirectLight(_streams);
        directLightingContribution += o21S267C0_o20S2C0_o14S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeDirectLightContribution(_streams);
        directLightingContribution += o21S267C0_o20S2C0_o19S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(_streams);
    }
    vec3 environmentLightingContribution = vec3(0.0);
    o3S423C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o21S267C0_o20S2C0_o14S2C0_MaterialSurfaceShadingDiffuseLambert_true__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o21S267C0_o20S2C0_o19S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id32 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id33 = _streams.matDiffuse_id12.w;
    o21S267C0_o20S2C0_o14S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
    o21S267C0_o20S2C0_o19S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
}

void o21S267C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o21S267C0_o5S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o21S267C0_o7S2C0_MaterialSurfaceNormalMap_false_true__Compute(_streams);
    o21S267C0_o9S2C0_MaterialSurfaceGlossinessMap_false__Compute(_streams);
    o21S267C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(_streams);
    o21S267C0_o13S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(_streams);
    o21S267C0_o20S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id27 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id9.xyz);
    _streams.shadingColor_id32 = vec3(0.0);
    o28S267C1_MaterialPixelShadingStream_ResetStream(_streams);
    o21S267C0_MaterialSurfaceArray_Compute(_streams);
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

void SharedTextureCoordinate_PSMain(inout PS_STREAMS _streams)
{
    _streams.TexCoord1_id49 = _streams.TexCoord_id48;
    _streams.TexCoord2_id50 = _streams.TexCoord_id48;
    _streams.TexCoord3_id51 = _streams.TexCoord_id48;
    _streams.TexCoord4_id52 = _streams.TexCoord_id48;
    _streams.TexCoord5_id53 = _streams.TexCoord_id48;
    _streams.TexCoord6_id54 = _streams.TexCoord_id48;
    _streams.TexCoord7_id55 = _streams.TexCoord_id48;
    _streams.TexCoord8_id56 = _streams.TexCoord_id48;
    _streams.TexCoord9_id57 = _streams.TexCoord_id48;
    NormalBase_PSMain(_streams);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), false, vec4(0.0), 0.0, vec3(0.0), vec3(0.0), vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, vec3(0.0), 0.0, vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), vec2(0.0), uvec2(0u), 0, vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.meshNormal_id4 = PS_IN_NORMAL;
    _streams.meshTangent_id6 = PS_IN_TANGENT;
    _streams.PositionWS_id9 = PS_IN_POSITION_WS;
    _streams.TexCoord_id48 = PS_IN_TEXCOORD0;
    _streams.ScreenPosition_id60 = PS_IN_SCREENPOSITION;
    _streams.IsFrontFace_id1 = PS_IN_SV_IsFrontFace;
    _streams.ScreenPosition_id60 /= vec4(_streams.ScreenPosition_id60.w);
    SharedTextureCoordinate_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

