#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    bool IsFrontFace_id1;
    vec4 ColorTarget_id2;
    vec3 normalWS_id3;
    mat3 tangentToWorld_id4;
    vec4 PositionWS_id5;
    vec3 matNormal_id6;
    vec4 matColorBase_id7;
    vec4 matDiffuse_id8;
    float matGlossiness_id9;
    vec3 matSpecular_id10;
    float matSpecularIntensity_id11;
    float matAmbientOcclusion_id12;
    float matAmbientOcclusionDirectLightingFactor_id13;
    float matCavity_id14;
    float matCavityDiffuse_id15;
    float matCavitySpecular_id16;
    vec4 matEmissive_id17;
    float matEmissiveIntensity_id18;
    vec2 matDiffuseSpecularAlphaBlend_id19;
    vec3 matAlphaBlendColor_id20;
    float matAlphaDiscard_id21;
    vec3 viewWS_id22;
    vec3 matDiffuseVisible_id23;
    float alphaRoughness_id24;
    vec3 matSpecularVisible_id25;
    float NdotV_id26;
    vec3 shadingColor_id27;
    float shadingColorAlpha_id28;
    vec3 lightPositionWS_id29;
    vec3 lightDirectionWS_id30;
    vec3 lightColor_id31;
    vec3 lightColorNdotL_id32;
    vec3 envLightDiffuseColor_id33;
    vec3 envLightSpecularColor_id34;
    float NdotL_id35;
    float lightDirectAmbientOcclusion_id36;
    float matBlend_id37;
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

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

layout(std140) uniform PerMaterial
{
    vec4 o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
} PerMaterial_var;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_normalWS;
layout(location = 2) in mat3 PS_IN_tangentToWorld;
layout(location = 3) in vec4 PS_IN_PositionWS;
layout(location = 4) in bool PS_IN_IsFrontFace;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id3, _streams.normalWS_id3) > 0.0)
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
    _streams.matBlend_id37 = 0.0;
}

void o6S245C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S245C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id6 = vec3(0.0, 0.0, 1.0);
    _streams.matColorBase_id7 = vec4(0.0);
    _streams.matDiffuse_id8 = vec4(0.0);
    _streams.matDiffuseVisible_id23 = vec3(0.0);
    _streams.matSpecular_id10 = vec3(0.0);
    _streams.matSpecularVisible_id25 = vec3(0.0);
    _streams.matSpecularIntensity_id11 = 1.0;
    _streams.matGlossiness_id9 = 0.0;
    _streams.alphaRoughness_id24 = 1.0;
    _streams.matAmbientOcclusion_id12 = 1.0;
    _streams.matAmbientOcclusionDirectLightingFactor_id13 = 0.0;
    _streams.matCavity_id14 = 1.0;
    _streams.matCavityDiffuse_id15 = 0.0;
    _streams.matCavitySpecular_id16 = 0.0;
    _streams.matEmissive_id17 = vec4(0.0);
    _streams.matEmissiveIntensity_id18 = 0.0;
    _streams.matDiffuseSpecularAlphaBlend_id19 = vec2(1.0);
    _streams.matAlphaBlendColor_id20 = vec3(1.0);
    _streams.matAlphaDiscard_id21 = 0.100000001490116119384765625;
}

void o6S245C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S245C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id28 = 1.0;
}

vec4 o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return PerMaterial_var.o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
}

void o5S245C0_o2S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o5S245C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id8 = colorBase;
    _streams.matColorBase_id7 = colorBase;
}

void o5S245C0_o4S2C0_NormalUpdate_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id3 = normalize(_streams.tangentToWorld_id4 * normalInTangentSpace);
}

void o5S245C0_o4S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id29 = vec3(0.0);
    _streams.lightDirectionWS_id30 = vec3(0.0);
    _streams.lightColor_id31 = vec3(0.0);
    _streams.lightColorNdotL_id32 = vec3(0.0);
    _streams.envLightDiffuseColor_id33 = vec3(0.0);
    _streams.envLightSpecularColor_id34 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id36 = 1.0;
    _streams.NdotL_id35 = 0.0;
}

void o5S245C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id36 = mix(1.0, _streams.matAmbientOcclusion_id12, _streams.matAmbientOcclusionDirectLightingFactor_id13);
    _streams.matDiffuseVisible_id23 = ((_streams.matDiffuse_id8.xyz * mix(1.0, _streams.matCavity_id14, _streams.matCavityDiffuse_id15)) * _streams.matDiffuseSpecularAlphaBlend_id19.x) * _streams.matAlphaBlendColor_id20;
    _streams.matSpecularVisible_id25 = (((_streams.matSpecular_id10 * _streams.matSpecularIntensity_id11) * mix(1.0, _streams.matCavity_id14, _streams.matCavitySpecular_id16)) * _streams.matDiffuseSpecularAlphaBlend_id19.y) * _streams.matAlphaBlendColor_id20;
    _streams.NdotV_id26 = max(dot(_streams.normalWS_id3, _streams.viewWS_id22), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id9;
    _streams.alphaRoughness_id24 = max(roughness * roughness, 0.001000000047497451305389404296875);
}

void o0S414C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id33 = vec3(0.0);
    _streams.envLightSpecularColor_id34 = vec3(0.0);
}

void o0S414C0_LightConstantWhite_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o0S414C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    _streams.envLightDiffuseColor_id33 = vec3(1.0);
    _streams.envLightSpecularColor_id34 = vec3(1.0);
}

vec3 o5S245C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id23;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id25);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id33;
}

void o5S245C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id6;
    o5S245C0_o4S2C0_NormalUpdate_UpdateNormalFromTangentSpace(_streams, param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id3 = -_streams.normalWS_id3;
    }
    o5S245C0_o4S2C0_LightStream_ResetLightStream(_streams);
    o5S245C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    vec3 directLightingContribution = vec3(0.0);
    vec3 environmentLightingContribution = vec3(0.0);
    o0S414C0_LightConstantWhite_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o5S245C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id27 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id28 = _streams.matDiffuse_id8.w;
}

void o5S245C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o5S245C0_o2S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o5S245C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id22 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id5.xyz);
    _streams.shadingColor_id27 = vec3(0.0);
    o6S245C1_MaterialPixelShadingStream_ResetStream(_streams);
    o5S245C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id27, _streams.shadingColorAlpha_id28);
}

vec4 EffectCompiling_Shading(inout PS_STREAMS _streams)
{
    float factor = (sin(PerFrame_var.Global_Time * 6.0) * 0.25) + 0.25;
    vec4 reloadColor = vec4(0.660000026226043701171875, 1.0, 0.25, 1.0);
    vec4 _111 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    return mix(_111, reloadColor, vec4(factor));
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _13 = EffectCompiling_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), false, vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0);
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id3 = PS_IN_normalWS;
    _streams.tangentToWorld_id4 = PS_IN_tangentToWorld;
    _streams.PositionWS_id5 = PS_IN_PositionWS;
    _streams.IsFrontFace_id1 = PS_IN_IsFrontFace;
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

