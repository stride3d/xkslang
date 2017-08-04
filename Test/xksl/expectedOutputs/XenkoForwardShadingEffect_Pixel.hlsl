struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float matBlend_id2;
    float3 meshNormal_id3;
    float4 meshTangent_id4;
    float3 normalWS_id5;
    float3x3 tangentToWorld_id6;
    float4 PositionWS_id7;
    float3 matNormal_id8;
    float4 matColorBase_id9;
    float4 matDiffuse_id10;
    float matGlossiness_id11;
    float3 matSpecular_id12;
    float matSpecularIntensity_id13;
    float matAmbientOcclusion_id14;
    float matAmbientOcclusionDirectLightingFactor_id15;
    float matCavity_id16;
    float matCavityDiffuse_id17;
    float matCavitySpecular_id18;
    float4 matEmissive_id19;
    float matEmissiveIntensity_id20;
    float2 matDiffuseSpecularAlphaBlend_id21;
    float3 matAlphaBlendColor_id22;
    float matAlphaDiscard_id23;
    float3 viewWS_id24;
    float3 matDiffuseVisible_id25;
    float alphaRoughness_id26;
    float3 matSpecularVisible_id27;
    float NdotV_id28;
    float3 shadingColor_id29;
    float shadingColorAlpha_id30;
    float3 H_id31;
    float NdotH_id32;
    float LdotH_id33;
    float VdotH_id34;
    float3 lightPositionWS_id35;
    float3 lightDirectionWS_id36;
    float3 lightColor_id37;
    float3 lightColorNdotL_id38;
    float3 envLightDiffuseColor_id39;
    float3 envLightSpecularColor_id40;
    float NdotL_id41;
    float lightDirectAmbientOcclusion_id42;
    float3 shadowColor_id43;
    float2 TexCoord_id44;
    float2 TexCoord_id45;
};

cbuffer PerDraw
{
    float4x4 Transformation_World;
    float4x4 Transformation_WorldInverse;
    float4x4 Transformation_WorldInverseTranspose;
    float4x4 Transformation_WorldView;
    float4x4 Transformation_WorldViewInverse;
    float4x4 Transformation_WorldViewProjection;
    float3 Transformation_WorldScale;
    float4 Transformation_EyeMS;
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
    float4 o0S450C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S450C0_LightDirectionalGroup_Lights[8];
    int o0S450C0_DirectLightGroupPerView_LightCount;
    float3 o1S435C0_LightSimpleAmbient_AmbientLight;
    float4 o1S435C0_LightSimpleAmbient__padding_PerView_Lighting;
};
cbuffer PerMaterial
{
    float o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat;
    float4 o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
    float o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat;
    float2 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float o24S34C0_o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_constantFloat;
};
cbuffer PerFrame
{
    float Global_Time;
    float Global_TimeStep;
};
SamplerState DynamicSampler_Sampler;
Texture2D<float4> DynamicTexture_Texture;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_meshNormal;
static float4 PS_IN_meshTangent;
static float4 PS_IN_PositionWS;
static float2 PS_IN_TexCoord;
static float2 PS_IN_TexCoord_1;
static float4 PS_OUT_ColorTarget;
static float PS_OUT_matBlend;
static float3 PS_OUT_normalWS;
static float3x3 PS_OUT_tangentToWorld;
static float3 PS_OUT_matNormal;
static float4 PS_OUT_matColorBase;
static float4 PS_OUT_matDiffuse;
static float PS_OUT_matGlossiness;
static float3 PS_OUT_matSpecular;
static float PS_OUT_matSpecularIntensity;
static float PS_OUT_matAmbientOcclusion;
static float PS_OUT_matAmbientOcclusionDirectLightingFactor;
static float PS_OUT_matCavity;
static float PS_OUT_matCavityDiffuse;
static float PS_OUT_matCavitySpecular;
static float4 PS_OUT_matEmissive;
static float PS_OUT_matEmissiveIntensity;
static float2 PS_OUT_matDiffuseSpecularAlphaBlend;
static float3 PS_OUT_matAlphaBlendColor;
static float PS_OUT_matAlphaDiscard;
static float3 PS_OUT_viewWS;
static float3 PS_OUT_matDiffuseVisible;
static float PS_OUT_alphaRoughness;
static float3 PS_OUT_matSpecularVisible;
static float PS_OUT_NdotV;
static float3 PS_OUT_shadingColor;
static float PS_OUT_shadingColorAlpha;
static float3 PS_OUT_H;
static float PS_OUT_NdotH;
static float PS_OUT_LdotH;
static float PS_OUT_VdotH;
static float3 PS_OUT_lightPositionWS;
static float3 PS_OUT_lightDirectionWS;
static float3 PS_OUT_lightColor;
static float3 PS_OUT_lightColorNdotL;
static float3 PS_OUT_envLightDiffuseColor;
static float3 PS_OUT_envLightSpecularColor;
static float PS_OUT_NdotL;
static float PS_OUT_lightDirectAmbientOcclusion;
static float3 PS_OUT_shadowColor;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_meshNormal : NORMAL;
    float4 PS_IN_meshTangent : TANGENT;
    float4 PS_IN_PositionWS : POSITION_WS;
    float2 PS_IN_TexCoord : TEXCOORD0;
    float2 PS_IN_TexCoord_1 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
    float PS_OUT_matBlend : SV_Target1;
    float3 PS_OUT_normalWS : NORMALWS;
    float3x3 PS_OUT_tangentToWorld : SV_Target3;
    float3 PS_OUT_matNormal : SV_Target4;
    float4 PS_OUT_matColorBase : SV_Target5;
    float4 PS_OUT_matDiffuse : SV_Target6;
    float PS_OUT_matGlossiness : SV_Target7;
    float3 PS_OUT_matSpecular : SV_Target8;
    float PS_OUT_matSpecularIntensity : SV_Target9;
    float PS_OUT_matAmbientOcclusion : SV_Target10;
    float PS_OUT_matAmbientOcclusionDirectLightingFactor : SV_Target11;
    float PS_OUT_matCavity : SV_Target12;
    float PS_OUT_matCavityDiffuse : SV_Target13;
    float PS_OUT_matCavitySpecular : SV_Target14;
    float4 PS_OUT_matEmissive : SV_Target15;
    float PS_OUT_matEmissiveIntensity : SV_Target16;
    float2 PS_OUT_matDiffuseSpecularAlphaBlend : SV_Target17;
    float3 PS_OUT_matAlphaBlendColor : SV_Target18;
    float PS_OUT_matAlphaDiscard : SV_Target19;
    float3 PS_OUT_viewWS : SV_Target20;
    float3 PS_OUT_matDiffuseVisible : SV_Target21;
    float PS_OUT_alphaRoughness : SV_Target22;
    float3 PS_OUT_matSpecularVisible : SV_Target23;
    float PS_OUT_NdotV : SV_Target24;
    float3 PS_OUT_shadingColor : SV_Target25;
    float PS_OUT_shadingColorAlpha : SV_Target26;
    float3 PS_OUT_H : SV_Target27;
    float PS_OUT_NdotH : SV_Target28;
    float PS_OUT_LdotH : SV_Target29;
    float PS_OUT_VdotH : SV_Target30;
    float3 PS_OUT_lightPositionWS : SV_Target31;
    float3 PS_OUT_lightDirectionWS : SV_Target32;
    float3 PS_OUT_lightColor : SV_Target33;
    float3 PS_OUT_lightColorNdotL : SV_Target34;
    float3 PS_OUT_envLightDiffuseColor : SV_Target35;
    float3 PS_OUT_envLightSpecularColor : SV_Target36;
    float PS_OUT_NdotL : SV_Target37;
    float PS_OUT_lightDirectAmbientOcclusion : SV_Target38;
    float3 PS_OUT_shadowColor : SV_Target39;
};

void NormalBase_GenerateNormal_PS()
{
}

float3x3 NormalStream_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id3 = normalize(_streams.meshNormal_id3);
    float3 tangent = normalize(_streams.meshTangent_id4.xyz);
    float3 bitangent = cross(_streams.meshNormal_id3, tangent) * _streams.meshTangent_id4.w;
    float3x3 tangentMatrix = float3x3(float3(tangent), float3(bitangent), float3(_streams.meshNormal_id3));
    return tangentMatrix;
}

float3x3 NormalFromNormalMapping_GetTangentWorldTransform()
{
    return float3x3(float3(Transformation_WorldInverseTranspose[0].x, Transformation_WorldInverseTranspose[0].y, Transformation_WorldInverseTranspose[0].z), float3(Transformation_WorldInverseTranspose[1].x, Transformation_WorldInverseTranspose[1].y, Transformation_WorldInverseTranspose[1].z), float3(Transformation_WorldInverseTranspose[2].x, Transformation_WorldInverseTranspose[2].y, Transformation_WorldInverseTranspose[2].z));
}

void NormalStream_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    float3x3 _63 = NormalStream_GetTangentMatrix(_streams);
    float3x3 tangentMatrix = _63;
    float3x3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id6 = mul(tangentMatrix, tangentWorldTransform);
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
    _streams.matBlend_id2 = 0.0f;
}

void o25S246C1_MaterialPixelStream_ResetStream(out PS_STREAMS _streams)
{
    o25S246C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id8 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id9 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuse_id10 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuseVisible_id25 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecular_id12 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularVisible_id27 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularIntensity_id13 = 1.0f;
    _streams.matGlossiness_id11 = 0.0f;
    _streams.alphaRoughness_id26 = 1.0f;
    _streams.matAmbientOcclusion_id14 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id15 = 0.0f;
    _streams.matCavity_id16 = 1.0f;
    _streams.matCavityDiffuse_id17 = 0.0f;
    _streams.matCavitySpecular_id18 = 0.0f;
    _streams.matEmissive_id19 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matEmissiveIntensity_id20 = 0.0f;
    _streams.matDiffuseSpecularAlphaBlend_id21 = float2(1.0f, 1.0f);
    _streams.matAlphaBlendColor_id22 = float3(1.0f, 1.0f, 1.0f);
    _streams.matAlphaDiscard_id23 = 0.0f;
}

void o25S246C1_MaterialPixelShadingStream_ResetStream(out PS_STREAMS _streams)
{
    o25S246C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id30 = 1.0f;
}

float4 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id44 * o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o18S246C0_o3S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id10 = colorBase;
    _streams.matColorBase_id9 = colorBase;
}

float2 o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    float _843 = sqrt(1.0f + (x * x));
    return float2(x, 1.0f) / float2(_843, _843);
}

float4 o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    float2 offset = _streams.TexCoord_id45 - float2(0.5f, 0.5f);
    float phase = length(offset);
    float derivative = cos((((phase + (Global_Time * -0.02999999932944774627685546875f)) * 2.0f) * 3.1400001049041748046875f) * 5.0f) * 0.100000001490116119384765625f;
    float param = offset.y / offset.x;
    float2 xz = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    float2 xy = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    float2 _823 = (((xz.yx * sign(offset.x)) * -xy.x) * 0.5f) + float2(0.5f, 0.5f);
    float3 normal;
    normal = float3(_823.x, _823.y, normal.z);
    normal.z = xy.y;
    return float4(normal, 1.0f);
}

void o18S246C0_o5S2C0_MaterialSurfaceNormalMap_false_true__Compute(inout PS_STREAMS _streams)
{
    float4 normal = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(_streams);
    if (true)
    {
        normal = (normal * 2.0f) - float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    if (false)
    {
        normal.z = sqrt(max(0.0f, 1.0f - ((normal.x * normal.x) + (normal.y * normal.y))));
    }
    _streams.matNormal_id8 = normal.xyz;
}

float4 o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute()
{
    return float4(o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o18S246C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(out PS_STREAMS _streams)
{
    float glossiness = o18S246C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__Compute().x;
    if (false)
    {
        glossiness = 1.0f - glossiness;
    }
    _streams.matGlossiness_id11 = glossiness;
}

float4 o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute()
{
    return o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
}

void o18S246C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecular_id12 = o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

float4 o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return float4(o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat, o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o18S246C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id13 = o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void o18S246C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, float3 normalInTangentSpace)
{
    _streams.normalWS_id5 = normalize(mul(normalInTangentSpace, _streams.tangentToWorld_id6));
}

void o18S246C0_o17S2C0_LightStream_ResetLightStream(out PS_STREAMS _streams)
{
    _streams.lightPositionWS_id35 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectionWS_id36 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColor_id37 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColorNdotL_id38 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightDiffuseColor_id39 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id40 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectAmbientOcclusion_id42 = 1.0f;
    _streams.NdotL_id41 = 0.0f;
}

float o18S246C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(PS_STREAMS _streams, float3 averageNormal)
{
    float r = length(_streams.matNormal_id8);
    float roughnessAdjust = 0.0f;
    if (r < 1.0f)
    {
        roughnessAdjust = (0.5f * (1.0f - (r * r))) / ((3.0f * r) - ((r * r) * r));
    }
    return roughnessAdjust;
}

void o18S246C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id42 = lerp(1.0f, _streams.matAmbientOcclusion_id14, _streams.matAmbientOcclusionDirectLightingFactor_id15);
    _streams.matDiffuseVisible_id25 = ((_streams.matDiffuse_id10.xyz * lerp(1.0f, _streams.matCavity_id16, _streams.matCavityDiffuse_id17)) * _streams.matDiffuseSpecularAlphaBlend_id21.x) * _streams.matAlphaBlendColor_id22;
    _streams.matSpecularVisible_id27 = (((_streams.matSpecular_id12 * _streams.matSpecularIntensity_id13) * lerp(1.0f, _streams.matCavity_id16, _streams.matCavitySpecular_id18)) * _streams.matDiffuseSpecularAlphaBlend_id21.y) * _streams.matAlphaBlendColor_id22;
    _streams.NdotV_id28 = max(dot(_streams.normalWS_id5, _streams.viewWS_id24), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id11;
    float3 param = _streams.matNormal_id8;
    float roughnessAdjust = o18S246C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id26 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875f);
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
    return o0S450C0_DirectLightGroupPerView_LightCount;
}

void o0S450C0_LightDirectionalGroup_8__PrepareDirectLightCore(out PS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id37 = o0S450C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id36 = -o0S450C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

void o0S450C0_ShadowGroup_ComputeShadow(out PS_STREAMS _streams, int lightIndex)
{
    _streams.shadowColor_id43 = float3(1.0f, 1.0f, 1.0f);
}

void o0S450C0_DirectLightGroup_PrepareDirectLight(inout PS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S450C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id41 = max(dot(_streams.normalWS_id5, _streams.lightDirectionWS_id36), 9.9999997473787516355514526367188e-05f);
    int param_1 = lightIndex;
    o0S450C0_ShadowGroup_ComputeShadow(_streams, param_1);
    _streams.lightColorNdotL_id38 = ((_streams.lightColor_id37 * _streams.shadowColor_id43) * _streams.NdotL_id41) * _streams.lightDirectAmbientOcclusion_id42;
}

void o18S246C0_o17S2C0_MaterialPixelShadingStream_PrepareMaterialPerDirectLight(inout PS_STREAMS _streams)
{
    _streams.H_id31 = normalize(_streams.viewWS_id24 + _streams.lightDirectionWS_id36);
    _streams.NdotH_id32 = max(dot(_streams.normalWS_id5, _streams.H_id31), 9.9999997473787516355514526367188e-05f);
    _streams.LdotH_id33 = max(dot(_streams.lightDirectionWS_id36, _streams.H_id31), 9.9999997473787516355514526367188e-05f);
    _streams.VdotH_id34 = _streams.LdotH_id33;
}

float3 o18S246C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (false)
    {
        diffuseColor *= (float3(1.0f, 1.0f, 1.0f) - _streams.matSpecularVisible_id27);
    }
    return ((diffuseColor / float3(3.1415927410125732421875f, 3.1415927410125732421875f, 3.1415927410125732421875f)) * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.x;
}

float3 o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float3 f90, float lOrVDotH)
{
    return f0 + ((f90 - f0) * pow(1.0f - lOrVDotH, 5.0f));
}

float3 o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(float3 f0, float lOrVDotH)
{
    float3 param = f0;
    float3 param_1 = float3(1.0f, 1.0f, 1.0f);
    float param_2 = lOrVDotH;
    return o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1, param_2);
}

float3 o18S246C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(PS_STREAMS _streams, float3 f0)
{
    float3 param = f0;
    float param_1 = _streams.LdotH_id33;
    return o18S246C0_o17S2C0_o16S2C0_o13S2C0_BRDFMicrofacet_FresnelSchlick(param, param_1);
}

float o18S246C0_o17S2C0_o16S2C0_o14S2C1_BRDFMicrofacet_VisibilityhSchlickGGX(float alphaR, float nDotX)
{
    float k = alphaR * 0.5f;
    return nDotX / ((nDotX * (1.0f - k)) + k);
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
    return alphaR2 / (3.1415927410125732421875f * pow(max(((nDotH * nDotH) * (alphaR2 - 1.0f)) + 1.0f, 9.9999997473787516355514526367188e-05f), 2.0f));
}

float o18S246C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(PS_STREAMS _streams)
{
    float param = _streams.alphaRoughness_id26;
    float param_1 = _streams.NdotH_id32;
    return o18S246C0_o17S2C0_o16S2C0_o15S2C2_BRDFMicrofacet_NormalDistributionGGX(param, param_1);
}

float3 o18S246C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeDirectLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id27;
    float3 param = specularColor;
    float3 fresnel = o18S246C0_o17S2C0_o16S2C0_o13S2C0_MaterialSpecularMicrofacetFresnelSchlick_Compute(_streams, param);
    float geometricShadowing = o18S246C0_o17S2C0_o16S2C0_o14S2C1_MaterialSpecularMicrofacetVisibilitySmithSchlickGGX_Compute(_streams);
    float normalDistribution = o18S246C0_o17S2C0_o16S2C0_o15S2C2_MaterialSpecularMicrofacetNormalDistributionGGX_Compute(_streams);
    float3 reflected = ((fresnel * geometricShadowing) * normalDistribution) / float3(4.0f, 4.0f, 4.0f);
    return (reflected * _streams.lightColorNdotL_id38) * _streams.matDiffuseSpecularAlphaBlend_id21.y;
}

void o1S435C0_EnvironmentLight_PrepareEnvironmentLight(out PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id39 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id40 = float3(0.0f, 0.0f, 0.0f);
}

void o1S435C0_LightSimpleAmbient_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o1S435C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    float3 lightColor = o1S435C0_LightSimpleAmbient_AmbientLight * _streams.matAmbientOcclusion_id14;
    _streams.envLightDiffuseColor_id39 = lightColor;
    _streams.envLightSpecularColor_id40 = lightColor;
}

float3 o18S246C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id25;
    if (false)
    {
        diffuseColor *= (float3(1.0f, 1.0f, 1.0f) - _streams.matSpecularVisible_id27);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id39;
}

float3 o18S246C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(float3 specularColor, float alphaR, float nDotV)
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

float3 o18S246C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 specularColor = _streams.matSpecularVisible_id27;
    float3 param = specularColor;
    float param_1 = _streams.alphaRoughness_id26;
    float param_2 = _streams.NdotV_id28;
    return o18S246C0_o17S2C0_o16S2C0_BRDFMicrofacet_EnvironmentLightingDFG_GGX_Fresnel_SmithSchlickGGX(param, param_1, param_2) * _streams.envLightSpecularColor_id40;
}

void o18S246C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id8;
    o18S246C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S246C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S246C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
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
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o1S435C0_LightSimpleAmbient_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o18S246C0_o17S2C0_o12S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    environmentLightingContribution += o18S246C0_o17S2C0_o16S2C0_MaterialSurfaceShadingSpecularMicrofacet_ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id29 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
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

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id24 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id7.xyz);
    _streams.shadingColor_id29 = float3(0.0f, 0.0f, 0.0f);
    o25S246C1_MaterialPixelShadingStream_ResetStream(_streams);
    o18S246C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id29, _streams.shadingColorAlpha_id30);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _12 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id1 = _12;
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), float2(0.0f, 0.0f), float2(0.0f, 0.0f) };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.meshNormal_id3 = PS_IN_meshNormal;
    _streams.meshTangent_id4 = PS_IN_meshTangent;
    _streams.PositionWS_id7 = PS_IN_PositionWS;
    _streams.TexCoord_id44 = PS_IN_TexCoord;
    _streams.TexCoord_id45 = PS_IN_TexCoord_1;
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    PS_OUT_matBlend = _streams.matBlend_id2;
    PS_OUT_normalWS = _streams.normalWS_id5;
    PS_OUT_tangentToWorld = _streams.tangentToWorld_id6;
    PS_OUT_matNormal = _streams.matNormal_id8;
    PS_OUT_matColorBase = _streams.matColorBase_id9;
    PS_OUT_matDiffuse = _streams.matDiffuse_id10;
    PS_OUT_matGlossiness = _streams.matGlossiness_id11;
    PS_OUT_matSpecular = _streams.matSpecular_id12;
    PS_OUT_matSpecularIntensity = _streams.matSpecularIntensity_id13;
    PS_OUT_matAmbientOcclusion = _streams.matAmbientOcclusion_id14;
    PS_OUT_matAmbientOcclusionDirectLightingFactor = _streams.matAmbientOcclusionDirectLightingFactor_id15;
    PS_OUT_matCavity = _streams.matCavity_id16;
    PS_OUT_matCavityDiffuse = _streams.matCavityDiffuse_id17;
    PS_OUT_matCavitySpecular = _streams.matCavitySpecular_id18;
    PS_OUT_matEmissive = _streams.matEmissive_id19;
    PS_OUT_matEmissiveIntensity = _streams.matEmissiveIntensity_id20;
    PS_OUT_matDiffuseSpecularAlphaBlend = _streams.matDiffuseSpecularAlphaBlend_id21;
    PS_OUT_matAlphaBlendColor = _streams.matAlphaBlendColor_id22;
    PS_OUT_matAlphaDiscard = _streams.matAlphaDiscard_id23;
    PS_OUT_viewWS = _streams.viewWS_id24;
    PS_OUT_matDiffuseVisible = _streams.matDiffuseVisible_id25;
    PS_OUT_alphaRoughness = _streams.alphaRoughness_id26;
    PS_OUT_matSpecularVisible = _streams.matSpecularVisible_id27;
    PS_OUT_NdotV = _streams.NdotV_id28;
    PS_OUT_shadingColor = _streams.shadingColor_id29;
    PS_OUT_shadingColorAlpha = _streams.shadingColorAlpha_id30;
    PS_OUT_H = _streams.H_id31;
    PS_OUT_NdotH = _streams.NdotH_id32;
    PS_OUT_LdotH = _streams.LdotH_id33;
    PS_OUT_VdotH = _streams.VdotH_id34;
    PS_OUT_lightPositionWS = _streams.lightPositionWS_id35;
    PS_OUT_lightDirectionWS = _streams.lightDirectionWS_id36;
    PS_OUT_lightColor = _streams.lightColor_id37;
    PS_OUT_lightColorNdotL = _streams.lightColorNdotL_id38;
    PS_OUT_envLightDiffuseColor = _streams.envLightDiffuseColor_id39;
    PS_OUT_envLightSpecularColor = _streams.envLightSpecularColor_id40;
    PS_OUT_NdotL = _streams.NdotL_id41;
    PS_OUT_lightDirectAmbientOcclusion = _streams.lightDirectAmbientOcclusion_id42;
    PS_OUT_shadowColor = _streams.shadowColor_id43;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_meshNormal = stage_input.PS_IN_meshNormal;
    PS_IN_meshTangent = stage_input.PS_IN_meshTangent;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_TexCoord_1 = stage_input.PS_IN_TexCoord_1;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    stage_output.PS_OUT_matBlend = PS_OUT_matBlend;
    stage_output.PS_OUT_normalWS = PS_OUT_normalWS;
    stage_output.PS_OUT_tangentToWorld = PS_OUT_tangentToWorld;
    stage_output.PS_OUT_matNormal = PS_OUT_matNormal;
    stage_output.PS_OUT_matColorBase = PS_OUT_matColorBase;
    stage_output.PS_OUT_matDiffuse = PS_OUT_matDiffuse;
    stage_output.PS_OUT_matGlossiness = PS_OUT_matGlossiness;
    stage_output.PS_OUT_matSpecular = PS_OUT_matSpecular;
    stage_output.PS_OUT_matSpecularIntensity = PS_OUT_matSpecularIntensity;
    stage_output.PS_OUT_matAmbientOcclusion = PS_OUT_matAmbientOcclusion;
    stage_output.PS_OUT_matAmbientOcclusionDirectLightingFactor = PS_OUT_matAmbientOcclusionDirectLightingFactor;
    stage_output.PS_OUT_matCavity = PS_OUT_matCavity;
    stage_output.PS_OUT_matCavityDiffuse = PS_OUT_matCavityDiffuse;
    stage_output.PS_OUT_matCavitySpecular = PS_OUT_matCavitySpecular;
    stage_output.PS_OUT_matEmissive = PS_OUT_matEmissive;
    stage_output.PS_OUT_matEmissiveIntensity = PS_OUT_matEmissiveIntensity;
    stage_output.PS_OUT_matDiffuseSpecularAlphaBlend = PS_OUT_matDiffuseSpecularAlphaBlend;
    stage_output.PS_OUT_matAlphaBlendColor = PS_OUT_matAlphaBlendColor;
    stage_output.PS_OUT_matAlphaDiscard = PS_OUT_matAlphaDiscard;
    stage_output.PS_OUT_viewWS = PS_OUT_viewWS;
    stage_output.PS_OUT_matDiffuseVisible = PS_OUT_matDiffuseVisible;
    stage_output.PS_OUT_alphaRoughness = PS_OUT_alphaRoughness;
    stage_output.PS_OUT_matSpecularVisible = PS_OUT_matSpecularVisible;
    stage_output.PS_OUT_NdotV = PS_OUT_NdotV;
    stage_output.PS_OUT_shadingColor = PS_OUT_shadingColor;
    stage_output.PS_OUT_shadingColorAlpha = PS_OUT_shadingColorAlpha;
    stage_output.PS_OUT_H = PS_OUT_H;
    stage_output.PS_OUT_NdotH = PS_OUT_NdotH;
    stage_output.PS_OUT_LdotH = PS_OUT_LdotH;
    stage_output.PS_OUT_VdotH = PS_OUT_VdotH;
    stage_output.PS_OUT_lightPositionWS = PS_OUT_lightPositionWS;
    stage_output.PS_OUT_lightDirectionWS = PS_OUT_lightDirectionWS;
    stage_output.PS_OUT_lightColor = PS_OUT_lightColor;
    stage_output.PS_OUT_lightColorNdotL = PS_OUT_lightColorNdotL;
    stage_output.PS_OUT_envLightDiffuseColor = PS_OUT_envLightDiffuseColor;
    stage_output.PS_OUT_envLightSpecularColor = PS_OUT_envLightSpecularColor;
    stage_output.PS_OUT_NdotL = PS_OUT_NdotL;
    stage_output.PS_OUT_lightDirectAmbientOcclusion = PS_OUT_lightDirectAmbientOcclusion;
    stage_output.PS_OUT_shadowColor = PS_OUT_shadowColor;
    return stage_output;
}
