struct PS_STREAMS
{
    float matBlend_id0;
    float4 ShadingPosition_id1;
    float4 ColorTarget_id2;
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
    float2 matDiffuseSpecularAlphaBlend_id19;
    float3 matAlphaBlendColor_id20;
    float3 viewWS_id21;
    float3 matDiffuseVisible_id22;
    float alphaRoughness_id23;
    float3 matSpecularVisible_id24;
    float NdotV_id25;
    float3 shadingColor_id26;
    float shadingColorAlpha_id27;
    float3 lightPositionWS_id28;
    float3 lightDirectionWS_id29;
    float3 lightColor_id30;
    float3 lightColorNdotL_id31;
    float3 envLightDiffuseColor_id32;
    float3 envLightSpecularColor_id33;
    float NdotL_id34;
    float lightDirectAmbientOcclusion_id35;
    float2 TexCoord_id36;
    float2 TexCoord_id37;
    float matDisplacement_id38;
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
    float4x4 o23S2C0_Transformation_WorldInverse;
    float4x4 o23S2C0_Transformation_WorldInverseTranspose;
    float4x4 o23S2C0_Transformation_WorldView;
    float4x4 o23S2C0_Transformation_WorldViewInverse;
    float4x4 o23S2C0_Transformation_WorldViewProjection;
    float3 o23S2C0_Transformation_WorldScale;
    float4 o23S2C0_Transformation_EyeMS;
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
};
cbuffer PerMaterial
{
    float o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat;
    float4 o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__constantColor;
    float o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat;
    float2 o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__scale;
    float2 o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__offset;
    float o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__constantFloat;
};
cbuffer PerFrame
{
    float Global_Time;
    float Global_TimeStep;
};
SamplerState o3S2C0_o2S2C0_DynamicSampler_Material_Sampler_i0_PerMaterial__Sampler;
Texture2D<float4> o3S2C0_o2S2C0_DynamicTexture_Material_DiffuseMap_PerMaterial__Texture;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_meshNormal;
static float4 PS_IN_meshTangent;
static float4 PS_IN_PositionWS;
static float PS_IN_matAmbientOcclusion;
static float PS_IN_matAmbientOcclusionDirectLightingFactor;
static float PS_IN_matCavity;
static float PS_IN_matCavityDiffuse;
static float PS_IN_matCavitySpecular;
static float2 PS_IN_matDiffuseSpecularAlphaBlend;
static float3 PS_IN_matAlphaBlendColor;
static float2 PS_IN_TexCoord;
static float2 PS_IN_TexCoord_1;
static float PS_OUT_matBlend;
static float4 PS_OUT_ColorTarget;
static float3 PS_OUT_normalWS;
static float3x3 PS_OUT_tangentToWorld;
static float3 PS_OUT_matNormal;
static float4 PS_OUT_matColorBase;
static float4 PS_OUT_matDiffuse;
static float PS_OUT_matGlossiness;
static float3 PS_OUT_matSpecular;
static float PS_OUT_matSpecularIntensity;
static float3 PS_OUT_viewWS;
static float3 PS_OUT_matDiffuseVisible;
static float PS_OUT_alphaRoughness;
static float3 PS_OUT_matSpecularVisible;
static float PS_OUT_NdotV;
static float3 PS_OUT_shadingColor;
static float PS_OUT_shadingColorAlpha;
static float3 PS_OUT_lightPositionWS;
static float3 PS_OUT_lightDirectionWS;
static float3 PS_OUT_lightColor;
static float3 PS_OUT_lightColorNdotL;
static float3 PS_OUT_envLightDiffuseColor;
static float3 PS_OUT_envLightSpecularColor;
static float PS_OUT_NdotL;
static float PS_OUT_lightDirectAmbientOcclusion;
static float PS_OUT_matDisplacement;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_meshNormal : NORMAL;
    float4 PS_IN_meshTangent : TANGENT;
    float4 PS_IN_PositionWS : POSITION_WS;
    float PS_IN_matAmbientOcclusion : TEXCOORD4;
    float PS_IN_matAmbientOcclusionDirectLightingFactor : TEXCOORD5;
    float PS_IN_matCavity : TEXCOORD6;
    float PS_IN_matCavityDiffuse : TEXCOORD7;
    float PS_IN_matCavitySpecular : TEXCOORD8;
    float2 PS_IN_matDiffuseSpecularAlphaBlend : TEXCOORD9;
    float3 PS_IN_matAlphaBlendColor : TEXCOORD10;
    float2 PS_IN_TexCoord : TEXCOORD0;
    float2 PS_IN_TexCoord_1 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float PS_OUT_matBlend : SV_Target0;
    float4 PS_OUT_ColorTarget : SV_Target0;
    float3 PS_OUT_normalWS : NORMALWS;
    float3x3 PS_OUT_tangentToWorld : SV_Target3;
    float3 PS_OUT_matNormal : SV_Target4;
    float4 PS_OUT_matColorBase : SV_Target5;
    float4 PS_OUT_matDiffuse : SV_Target6;
    float PS_OUT_matGlossiness : SV_Target7;
    float3 PS_OUT_matSpecular : SV_Target8;
    float PS_OUT_matSpecularIntensity : SV_Target9;
    float3 PS_OUT_viewWS : SV_Target10;
    float3 PS_OUT_matDiffuseVisible : SV_Target11;
    float PS_OUT_alphaRoughness : SV_Target12;
    float3 PS_OUT_matSpecularVisible : SV_Target13;
    float PS_OUT_NdotV : SV_Target14;
    float3 PS_OUT_shadingColor : SV_Target15;
    float PS_OUT_shadingColorAlpha : SV_Target16;
    float3 PS_OUT_lightPositionWS : SV_Target17;
    float3 PS_OUT_lightDirectionWS : SV_Target18;
    float3 PS_OUT_lightColor : SV_Target19;
    float3 PS_OUT_lightColorNdotL : SV_Target20;
    float3 PS_OUT_envLightDiffuseColor : SV_Target21;
    float3 PS_OUT_envLightSpecularColor : SV_Target22;
    float PS_OUT_NdotL : SV_Target23;
    float PS_OUT_lightDirectAmbientOcclusion : SV_Target24;
    float PS_OUT_matDisplacement : SV_Target25;
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
    _streams.matBlend_id0 = 0.0f;
}

void o25S246C1_MaterialDisplacementStream_ResetStream(out PS_STREAMS _streams)
{
    o25S246C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id38 = 0.0f;
}

float4 o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return o3S2C0_o2S2C0_DynamicTexture_Material_DiffuseMap_PerMaterial__Texture.Sample(o3S2C0_o2S2C0_DynamicSampler_Material_Sampler_i0_PerMaterial__Sampler, (_streams.TexCoord_id36 * o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__scale) + o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__offset);
}

void o18S246C0_o3S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o18S246C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id10 = colorBase;
    _streams.matColorBase_id9 = colorBase;
}

float2 o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    float _439 = sqrt(1.0f + (x * x));
    return float2(x, 1.0f) / float2(_439, _439);
}

float4 o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    float2 offset = _streams.TexCoord_id37 - float2(0.5f, 0.5f);
    float phase = length(offset);
    float derivative = cos((((phase + (Global_Time * -0.02999999932944774627685546875f)) * 2.0f) * 3.1400001049041748046875f) * 5.0f) * 0.100000001490116119384765625f;
    float param = offset.y / offset.x;
    float2 xz = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    float2 xy = o18S246C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    float2 _419 = (((xz.yx * sign(offset.x)) * -xy.x) * 0.5f) + float2(0.5f, 0.5f);
    float3 normal;
    normal = float3(_419.x, _419.y, normal.z);
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
    return float4(o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat, o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_GlossinessValue__constantFloat);
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
    return o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__constantColor;
}

void o18S246C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecular_id12 = o18S246C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

float4 o18S246C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return float4(o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat, o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__constantFloat);
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
    _streams.lightPositionWS_id28 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectionWS_id29 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColor_id30 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColorNdotL_id31 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightDiffuseColor_id32 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id33 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectAmbientOcclusion_id35 = 1.0f;
    _streams.NdotL_id34 = 0.0f;
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
    _streams.lightDirectAmbientOcclusion_id35 = lerp(1.0f, _streams.matAmbientOcclusion_id14, _streams.matAmbientOcclusionDirectLightingFactor_id15);
    _streams.matDiffuseVisible_id22 = ((_streams.matDiffuse_id10.xyz * lerp(1.0f, _streams.matCavity_id16, _streams.matCavityDiffuse_id17)) * _streams.matDiffuseSpecularAlphaBlend_id19.x) * _streams.matAlphaBlendColor_id20;
    _streams.matSpecularVisible_id24 = (((_streams.matSpecular_id12 * _streams.matSpecularIntensity_id13) * lerp(1.0f, _streams.matCavity_id16, _streams.matCavitySpecular_id18)) * _streams.matDiffuseSpecularAlphaBlend_id19.y) * _streams.matAlphaBlendColor_id20;
    _streams.NdotV_id25 = max(dot(_streams.normalWS_id5, _streams.viewWS_id21), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id11;
    float3 param = _streams.matNormal_id8;
    float roughnessAdjust = o18S246C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id23 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875f);
}

void o18S246C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id8;
    o18S246C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S246C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S246C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    _streams.shadingColor_id26 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
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

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id21 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id7.xyz);
    _streams.shadingColor_id26 = float3(0.0f, 0.0f, 0.0f);
    o25S246C1_MaterialDisplacementStream_ResetStream(_streams);
    o18S246C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _15 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id2 = _15;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, float2(0.0f, 0.0f), float2(0.0f, 0.0f), 0.0f };
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

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_meshNormal = stage_input.PS_IN_meshNormal;
    PS_IN_meshTangent = stage_input.PS_IN_meshTangent;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_matAmbientOcclusion = stage_input.PS_IN_matAmbientOcclusion;
    PS_IN_matAmbientOcclusionDirectLightingFactor = stage_input.PS_IN_matAmbientOcclusionDirectLightingFactor;
    PS_IN_matCavity = stage_input.PS_IN_matCavity;
    PS_IN_matCavityDiffuse = stage_input.PS_IN_matCavityDiffuse;
    PS_IN_matCavitySpecular = stage_input.PS_IN_matCavitySpecular;
    PS_IN_matDiffuseSpecularAlphaBlend = stage_input.PS_IN_matDiffuseSpecularAlphaBlend;
    PS_IN_matAlphaBlendColor = stage_input.PS_IN_matAlphaBlendColor;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_TexCoord_1 = stage_input.PS_IN_TexCoord_1;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_matBlend = PS_OUT_matBlend;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    stage_output.PS_OUT_normalWS = PS_OUT_normalWS;
    stage_output.PS_OUT_tangentToWorld = PS_OUT_tangentToWorld;
    stage_output.PS_OUT_matNormal = PS_OUT_matNormal;
    stage_output.PS_OUT_matColorBase = PS_OUT_matColorBase;
    stage_output.PS_OUT_matDiffuse = PS_OUT_matDiffuse;
    stage_output.PS_OUT_matGlossiness = PS_OUT_matGlossiness;
    stage_output.PS_OUT_matSpecular = PS_OUT_matSpecular;
    stage_output.PS_OUT_matSpecularIntensity = PS_OUT_matSpecularIntensity;
    stage_output.PS_OUT_viewWS = PS_OUT_viewWS;
    stage_output.PS_OUT_matDiffuseVisible = PS_OUT_matDiffuseVisible;
    stage_output.PS_OUT_alphaRoughness = PS_OUT_alphaRoughness;
    stage_output.PS_OUT_matSpecularVisible = PS_OUT_matSpecularVisible;
    stage_output.PS_OUT_NdotV = PS_OUT_NdotV;
    stage_output.PS_OUT_shadingColor = PS_OUT_shadingColor;
    stage_output.PS_OUT_shadingColorAlpha = PS_OUT_shadingColorAlpha;
    stage_output.PS_OUT_lightPositionWS = PS_OUT_lightPositionWS;
    stage_output.PS_OUT_lightDirectionWS = PS_OUT_lightDirectionWS;
    stage_output.PS_OUT_lightColor = PS_OUT_lightColor;
    stage_output.PS_OUT_lightColorNdotL = PS_OUT_lightColorNdotL;
    stage_output.PS_OUT_envLightDiffuseColor = PS_OUT_envLightDiffuseColor;
    stage_output.PS_OUT_envLightSpecularColor = PS_OUT_envLightSpecularColor;
    stage_output.PS_OUT_NdotL = PS_OUT_NdotL;
    stage_output.PS_OUT_lightDirectAmbientOcclusion = PS_OUT_lightDirectAmbientOcclusion;
    stage_output.PS_OUT_matDisplacement = PS_OUT_matDisplacement;
    return stage_output;
}
