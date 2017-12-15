struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    bool IsFrontFace_id1;
    float4 ColorTarget_id2;
    float3 normalWS_id3;
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
    column_major float4x4 Transformation_View;
    column_major float4x4 Transformation_ViewInverse;
    column_major float4x4 Transformation_Projection;
    column_major float4x4 Transformation_ProjectionInverse;
    column_major float4x4 Transformation_ViewProjection;
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
    float4 o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
};

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_normalWS;
static float4 PS_IN_PositionWS;
static bool PS_IN_IsFrontFace;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_normalWS : NORMALWS;
    float4 PS_IN_PositionWS : POSITION_WS;
    bool PS_IN_IsFrontFace : SV_IsFrontFace;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id3, _streams.normalWS_id3) > 0.0f)
    {
        _streams.normalWS_id3 = normalize(_streams.normalWS_id3);
    }
}

void ShaderBase_PSMain()
{
}

void o6S245C1_IStreamInitializer_ResetStream()
{
}

void o6S245C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S245C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id36 = 0.0f;
}

void o6S245C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S245C1_MaterialStream_ResetStream(_streams);
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
    _streams.matAlphaDiscard_id20 = 0.100000001490116119384765625f;
}

void o6S245C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S245C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id27 = 1.0f;
}

float4 o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
}

void o5S245C0_o2S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    float4 colorBase = o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id7 = colorBase;
    _streams.matColorBase_id6 = colorBase;
}

void NormalFromMesh_UpdateNormalFromTangentSpace(float3 normalInTangentSpace)
{
}

void o5S245C0_o4S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
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

void o5S245C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id35 = lerp(1.0f, _streams.matAmbientOcclusion_id11, _streams.matAmbientOcclusionDirectLightingFactor_id12);
    _streams.matDiffuseVisible_id22 = ((_streams.matDiffuse_id7.xyz * lerp(1.0f, _streams.matCavity_id13, _streams.matCavityDiffuse_id14)) * _streams.matDiffuseSpecularAlphaBlend_id18.x) * _streams.matAlphaBlendColor_id19;
    _streams.matSpecularVisible_id24 = (((_streams.matSpecular_id9 * _streams.matSpecularIntensity_id10) * lerp(1.0f, _streams.matCavity_id13, _streams.matCavitySpecular_id15)) * _streams.matDiffuseSpecularAlphaBlend_id18.y) * _streams.matAlphaBlendColor_id19;
    _streams.NdotV_id25 = max(dot(_streams.normalWS_id3, _streams.viewWS_id21), 9.9999997473787516355514526367188e-05f);
    float roughness = 1.0f - _streams.matGlossiness_id8;
    _streams.alphaRoughness_id23 = max(roughness * roughness, 0.001000000047497451305389404296875f);
}

void o0S414C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id32 = float3(0.0f, 0.0f, 0.0f);
    _streams.envLightSpecularColor_id33 = float3(0.0f, 0.0f, 0.0f);
}

void o0S414C0_LightConstantWhite_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o0S414C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    _streams.envLightDiffuseColor_id32 = float3(1.0f, 1.0f, 1.0f);
    _streams.envLightSpecularColor_id33 = float3(1.0f, 1.0f, 1.0f);
}

float3 o5S245C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    float3 diffuseColor = _streams.matDiffuseVisible_id22;
    if (false)
    {
        diffuseColor *= (1.0f.xxx - _streams.matSpecularVisible_id24);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id32;
}

void o5S245C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    float3 param = _streams.matNormal_id5;
    NormalFromMesh_UpdateNormalFromTangentSpace(param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id3 = -_streams.normalWS_id3;
    }
    o5S245C0_o4S2C0_LightStream_ResetLightStream(_streams);
    o5S245C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    float3 directLightingContribution = float3(0.0f, 0.0f, 0.0f);
    float3 environmentLightingContribution = float3(0.0f, 0.0f, 0.0f);
    o0S414C0_LightConstantWhite_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o5S245C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id26 += ((directLightingContribution * 3.1415927410125732421875f) + environmentLightingContribution);
    _streams.shadingColorAlpha_id27 = _streams.matDiffuse_id7.w;
}

void o5S245C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o5S245C0_o2S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o5S245C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id21 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id4.xyz);
    _streams.shadingColor_id26 = float3(0.0f, 0.0f, 0.0f);
    o6S245C1_MaterialPixelShadingStream_ResetStream(_streams);
    o5S245C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

float4 EffectCompiling_Shading(inout PS_STREAMS _streams)
{
    float factor = (sin(Global_Time * 6.0f) * 0.25f) + 0.25f;
    float4 reloadColor = float4(0.660000026226043701171875f, 1.0f, 0.25f, 1.0f);
    float4 _113 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    return lerp(_113, reloadColor, factor.xxxx);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _13 = EffectCompiling_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), false, float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id3 = PS_IN_normalWS;
    _streams.PositionWS_id4 = PS_IN_PositionWS;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_normalWS = stage_input.PS_IN_normalWS;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_IsFrontFace = stage_input.PS_IN_IsFrontFace;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
