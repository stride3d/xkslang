struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float3 normalWS_id2;
    float3x3 tangentToWorld_id3;
    float4 PositionWS_id4;
    float3 matNormal_id5;
    float4 matColorBase_id6;
    float4 matDiffuse_id7;
    float matGlossiness_id8;
    float3 matSpecular_id9;
    float matSpecularIntensity_id10;
    float matAmbientOcclusion_id11;
    float matAmbientOcclusionDirectLightingFactor_id12;
    float matCavity_id13;
    float matCavityDiffuse_id14;
    float matCavitySpecular_id15;
    float4 matEmissive_id16;
    float matEmissiveIntensity_id17;
    float2 matDiffuseSpecularAlphaBlend_id18;
    float3 matAlphaBlendColor_id19;
    float matAlphaDiscard_id20;
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
    float matBlend_id36;
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
cbuffer PerFrame
{
    float Global_Time;
    float Global_TimeStep;
};
cbuffer PerMaterial
{
    float4 o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
};

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_normalWS;
static float3x3 PS_IN_tangentToWorld;
static float4 PS_IN_PositionWS;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_normalWS : NORMALWS;
    float3 PS_IN_tangentToWorld_0 : TEXCOORD2;
    float3 PS_IN_tangentToWorld_1 : TEXCOORD3;
    float3 PS_IN_tangentToWorld_2 : TEXCOORD4;
    float4 PS_IN_PositionWS : POSITION_WS;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id2, _streams.normalWS_id2) > 0.0f)
    {
        _streams.normalWS_id2 = normalize(_streams.normalWS_id2);
    }
}

void ShaderBase_PSMain()
{
}

void o6S244C1_IStreamInitializer_ResetStream()
{
}

void o6S244C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S244C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id36 = 0.0f;
}

void o6S244C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S244C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id5 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id6 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuse_id7 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuseVisible_id22 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecular_id9 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularVisible_id24 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularIntensity_id10 = 1.0f;
    _streams.matGlossiness_id8 = 0.0f;
    _streams.alphaRoughness_id23 = 1.0f;
    _streams.matAmbientOcclusion_id11 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id12 = 0.0f;
    _streams.matCavity_id13 = 1.0f;
    _streams.matCavityDiffuse_id14 = 0.0f;
    _streams.matCavitySpecular_id15 = 0.0f;
    _streams.matEmissive_id16 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matEmissiveIntensity_id17 = 0.0f;
    _streams.matDiffuseSpecularAlphaBlend_id18 = float2(1.0f, 1.0f);
    _streams.matAlphaBlendColor_id19 = float3(1.0f, 1.0f, 1.0f);
    _streams.matAlphaDiscard_id20 = 0.0f;
}

void o6S244C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S244C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id27 = 1.0f;
}

float4 o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
}

void o5S244C0_o2S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id7 = colorBase;
    _streams.matColorBase_id6 = colorBase;
}

void o5S244C0_o4S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, float3 normalInTangentSpace)
{
    _streams.normalWS_id2 = normalize(mul(normalInTangentSpace, _streams.tangentToWorld_id3));
}

void o5S244C0_o4S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
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

float o5S244C0_o4S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(PS_STREAMS _streams, float3 averageNormal)
{
    float r = length(_streams.matNormal_id5);
    float roughnessAdjust = 0.0f;
    if (r < 1.0f)
    {
        roughnessAdjust = (0.5f * (1.0f - (r * r))) / ((3.0f * r) - ((r * r) * r));
    }
    return roughnessAdjust;
}

void o5S244C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id35 = lerp(1.0f, _streams.matAmbientOcclusion_id11, _streams.matAmbientOcclusionDirectLightingFactor_id12);
    _streams.matDiffuseVisible_id22 = ((_streams.matDiffuse_id7.xyz * lerp(1.0f, _streams.matCavity_id13, _streams.matCavityDiffuse_id14)) * _streams.matDiffuseSpecularAlphaBlend_id18.x) * _streams.matAlphaBlendColor_id19;
    _streams.matSpecularVisible_id24 = (((_streams.matSpecular_id9 * _streams.matSpecularIntensity_id10) * lerp(1.0f, _streams.matCavity_id13, _streams.matCavitySpecular_id15)) * _streams.matDiffuseSpecularAlphaBlend_id18.y) * _streams.matAlphaBlendColor_id19;
    _streams.NdotV_id25 = max(dot(_streams.normalWS_id2, _streams.viewWS_id21), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id8;
    float3 param = _streams.matNormal_id5;
    float roughnessAdjust = o5S244C0_o4S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id23 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875f);
}

void o0S447C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id32 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id33 = float3(0.0f, 0.0f, 0.0f);
}

void o0S447C0_LightConstantWhite_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o0S447C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    _streams.envLightDiffuseColor_id32 = float3(1.0f, 1.0f, 1.0f);
    _streams.envLightSpecularColor_id33 = float3(1.0f, 1.0f, 1.0f);
}

float3 o5S244C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id22;
    if (false)
    {
        diffuseColor *= (float3(1.0f, 1.0f, 1.0f) - _streams.matSpecularVisible_id24);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id32;
}

void o5S244C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id5;
    o5S244C0_o4S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o5S244C0_o4S2C0_LightStream_ResetLightStream(_streams);
    o5S244C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o0S447C0_LightConstantWhite_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o5S244C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id26 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id27 = _streams.matDiffuse_id7.w;
}

void o5S244C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o5S244C0_o2S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o5S244C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id21 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id4.xyz);
    _streams.shadingColor_id26 = float3(0.0f, 0.0f, 0.0f);
    o6S244C1_MaterialPixelShadingStream_ResetStream(_streams);
    o5S244C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

float4 EffectCompiling_Shading(inout PS_STREAMS _streams)
{
    float factor = (sin(Global_Time * 6.0f) * 0.25f) + 0.25f;
    float4 reloadColor = float4(0.660000026226043701171875f, 1.0f, 0.25f, 1.0f);
    float4 _112 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    return lerp(_112, reloadColor, float4(factor, factor, factor, factor));
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _12 = EffectCompiling_Shading(_streams);
    _streams.ColorTarget_id1 = _12;
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id2 = PS_IN_normalWS;
    _streams.tangentToWorld_id3 = PS_IN_tangentToWorld;
    _streams.PositionWS_id4 = PS_IN_PositionWS;
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_normalWS = stage_input.PS_IN_normalWS;
    PS_IN_tangentToWorld[0] = stage_input.PS_IN_tangentToWorld_0;
    PS_IN_tangentToWorld[1] = stage_input.PS_IN_tangentToWorld_1;
    PS_IN_tangentToWorld[2] = stage_input.PS_IN_tangentToWorld_2;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
