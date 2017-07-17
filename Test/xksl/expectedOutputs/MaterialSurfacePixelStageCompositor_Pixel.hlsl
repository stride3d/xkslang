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
    float4 ShadingPosition_id21;
    float4 ColorTarget_id22;
    float3 lightPositionWS_id23;
    float3 lightDirectionWS_id24;
    float3 lightColor_id25;
    float3 lightColorNdotL_id26;
    float3 envLightDiffuseColor_id27;
    float3 envLightSpecularColor_id28;
    float NdotL_id29;
    float lightDirectAmbientOcclusion_id30;
    float3 normalWS_id31;
    float3x3 tangentToWorld_id32;
    float matBlend_id33;
    float2 TexCoord_id34;
    float2 TexCoord_id35;
    float matDisplacement_id36;
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
    float ComputeColorConstantFloatLink_constantFloat;
    float4 ComputeColorConstantColorLink_constantColor;
    float2 ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 ComputeColorTextureScaledOffsetDynamicSampler_offset;
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
static float2 PS_IN_TexCoord_1;
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
static float4 PS_OUT_ColorTarget;
static float3 PS_OUT_lightPositionWS;
static float3 PS_OUT_lightDirectionWS;
static float3 PS_OUT_lightColor;
static float3 PS_OUT_lightColorNdotL;
static float3 PS_OUT_envLightDiffuseColor;
static float3 PS_OUT_envLightSpecularColor;
static float PS_OUT_NdotL;
static float PS_OUT_lightDirectAmbientOcclusion;
static float3 PS_OUT_normalWS;
static float PS_OUT_matBlend;
static float PS_OUT_matDisplacement;

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
    float2 PS_IN_TexCoord_1 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float3 PS_OUT_matNormal : SV_Target0;
    float4 PS_OUT_matColorBase : SV_Target1;
    float4 PS_OUT_matDiffuse : SV_Target2;
    float PS_OUT_matGlossiness : SV_Target3;
    float3 PS_OUT_matSpecular : SV_Target4;
    float PS_OUT_matSpecularIntensity : SV_Target5;
    float3 PS_OUT_viewWS : SV_Target6;
    float3 PS_OUT_matDiffuseVisible : SV_Target7;
    float PS_OUT_alphaRoughness : SV_Target8;
    float3 PS_OUT_matSpecularVisible : SV_Target9;
    float PS_OUT_NdotV : SV_Target10;
    float3 PS_OUT_shadingColor : SV_Target11;
    float PS_OUT_shadingColorAlpha : SV_Target12;
    float4 PS_OUT_ColorTarget : SV_Target0;
    float3 PS_OUT_lightPositionWS : SV_Target14;
    float3 PS_OUT_lightDirectionWS : SV_Target15;
    float3 PS_OUT_lightColor : SV_Target16;
    float3 PS_OUT_lightColorNdotL : SV_Target17;
    float3 PS_OUT_envLightDiffuseColor : SV_Target18;
    float3 PS_OUT_envLightSpecularColor : SV_Target19;
    float PS_OUT_NdotL : SV_Target20;
    float PS_OUT_lightDirectAmbientOcclusion : SV_Target21;
    float3 PS_OUT_normalWS : NORMALWS;
    float PS_OUT_matBlend : SV_Target23;
    float PS_OUT_matDisplacement : SV_Target24;
};

void ShaderBase_PSMain()
{
}

void o19S2C1_IStreamInitializer_ResetStream()
{
}

void o19S2C1_MaterialStream_ResetStream(out PS_STREAMS _streams)
{
    o19S2C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id33 = 0.0f;
}

void o19S2C1_MaterialDisplacementStream_ResetStream(out PS_STREAMS _streams)
{
    o19S2C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id36 = 0.0f;
}

float4 o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id34 * ComputeColorTextureScaledOffsetDynamicSampler_scale) + ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

void o18S2C0_o3S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o18S2C0_o3S2C0_o2S2C0_ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    _streams.matDiffuse_id2 = colorBase;
    _streams.matColorBase_id1 = colorBase;
}

float2 o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(float x)
{
    float _361 = sqrt(1.0f + (x * x));
    return float2(x, 1.0f) / float2(_361, _361);
}

float4 o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__Compute(PS_STREAMS _streams)
{
    float2 offset = _streams.TexCoord_id35 - float2(0.5f, 0.5f);
    float phase = length(offset);
    float derivative = cos((((phase + (Global_Time * -0.02999999932944774627685546875f)) * 2.0f) * 3.1400001049041748046875f) * 5.0f) * 0.100000001490116119384765625f;
    float param = offset.y / offset.x;
    float2 xz = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param);
    float param_1 = derivative;
    float2 xy = o18S2C0_o5S2C0_o4S2C0_ComputeColorWaveNormal_5_0_1__0_03__SincosOfAtan(param_1);
    float2 _341 = (((xz.yx * sign(offset.x)) * -xy.x) * 0.5f) + float2(0.5f, 0.5f);
    float3 normal;
    normal = float3(_341.x, _341.y, normal.z);
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
    return float4(ComputeColorConstantFloatLink_constantFloat, ComputeColorConstantFloatLink_constantFloat, ComputeColorConstantFloatLink_constantFloat, ComputeColorConstantFloatLink_constantFloat);
}

void o18S2C0_o7S2C0_MaterialSurfaceGlossinessMap_false__Compute(out PS_STREAMS _streams)
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
    return ComputeColorConstantColorLink_constantColor;
}

void o18S2C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecular_rgb__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecular_id4 = o18S2C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_SpecularValue__Compute().xyz;
}

float4 o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute()
{
    return float4(ComputeColorConstantFloatLink_constantFloat, ComputeColorConstantFloatLink_constantFloat, ComputeColorConstantFloatLink_constantFloat, ComputeColorConstantFloatLink_constantFloat);
}

void o18S2C0_o11S2C0_MaterialSurfaceSetStreamFromComputeColor_matSpecularIntensity_r__Compute(out PS_STREAMS _streams)
{
    _streams.matSpecularIntensity_id5 = o18S2C0_o11S2C0_o10S2C0_ComputeColorConstantFloatLink_Material_SpecularIntensityValue__Compute().x;
}

void o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, float3 normalInTangentSpace)
{
    _streams.normalWS_id31 = normalize(mul(normalInTangentSpace, _streams.tangentToWorld_id32));
}

void o18S2C0_o17S2C0_LightStream_ResetLightStream(out PS_STREAMS _streams)
{
    _streams.lightPositionWS_id23 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectionWS_id24 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColor_id25 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightColorNdotL_id26 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightDiffuseColor_id27 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id28 = float3(0.0f, 0.0f, 0.0f);
    _streams.lightDirectAmbientOcclusion_id30 = 1.0f;
    _streams.NdotL_id29 = 0.0f;
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
    _streams.lightDirectAmbientOcclusion_id30 = lerp(1.0f, _streams.matAmbientOcclusion_id6, _streams.matAmbientOcclusionDirectLightingFactor_id7);
    _streams.matDiffuseVisible_id14 = ((_streams.matDiffuse_id2.xyz * lerp(1.0f, _streams.matCavity_id8, _streams.matCavityDiffuse_id9)) * _streams.matDiffuseSpecularAlphaBlend_id11.x) * _streams.matAlphaBlendColor_id12;
    _streams.matSpecularVisible_id16 = (((_streams.matSpecular_id4 * _streams.matSpecularIntensity_id5) * lerp(1.0f, _streams.matCavity_id8, _streams.matCavitySpecular_id10)) * _streams.matDiffuseSpecularAlphaBlend_id11.y) * _streams.matAlphaBlendColor_id12;
    _streams.NdotV_id17 = max(dot(_streams.normalWS_id31, _streams.viewWS_id13), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id3;
    float3 param = _streams.matNormal_id0;
    float roughnessAdjust = o18S2C0_o17S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id15 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875f);
}

void o18S2C0_o17S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id0;
    o18S2C0_o17S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o18S2C0_o17S2C0_LightStream_ResetLightStream(_streams);
    o18S2C0_o17S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    _streams.shadingColor_id19 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
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
    PS_STREAMS _streams = { float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, float3(0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), 0.0f, float2(0.0f, 0.0f), float2(0.0f, 0.0f), 0.0f };
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
    float4 _27 = MaterialSurfacePixelStageCompositor_Shading(_streams);
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
    PS_IN_TexCoord_1 = stage_input.PS_IN_TexCoord_1;
    frag_main();
    SPIRV_Cross_Output stage_output;
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
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    stage_output.PS_OUT_lightPositionWS = PS_OUT_lightPositionWS;
    stage_output.PS_OUT_lightDirectionWS = PS_OUT_lightDirectionWS;
    stage_output.PS_OUT_lightColor = PS_OUT_lightColor;
    stage_output.PS_OUT_lightColorNdotL = PS_OUT_lightColorNdotL;
    stage_output.PS_OUT_envLightDiffuseColor = PS_OUT_envLightDiffuseColor;
    stage_output.PS_OUT_envLightSpecularColor = PS_OUT_envLightSpecularColor;
    stage_output.PS_OUT_NdotL = PS_OUT_NdotL;
    stage_output.PS_OUT_lightDirectAmbientOcclusion = PS_OUT_lightDirectAmbientOcclusion;
    stage_output.PS_OUT_normalWS = PS_OUT_normalWS;
    stage_output.PS_OUT_matBlend = PS_OUT_matBlend;
    stage_output.PS_OUT_matDisplacement = PS_OUT_matDisplacement;
    return stage_output;
}
