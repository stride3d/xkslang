#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    vec3 normalWS_id2;
    mat3 tangentToWorld_id3;
    vec4 PositionWS_id4;
    vec3 matNormal_id5;
    vec4 matColorBase_id6;
    vec4 matDiffuse_id7;
    float matGlossiness_id8;
    vec3 matSpecular_id9;
    float matSpecularIntensity_id10;
    float matAmbientOcclusion_id11;
    float matAmbientOcclusionDirectLightingFactor_id12;
    float matCavity_id13;
    float matCavityDiffuse_id14;
    float matCavitySpecular_id15;
    vec4 matEmissive_id16;
    float matEmissiveIntensity_id17;
    vec2 matDiffuseSpecularAlphaBlend_id18;
    vec3 matAlphaBlendColor_id19;
    float matAlphaDiscard_id20;
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
    float matBlend_id36;
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
    vec4 o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
} PerMaterial_var;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_normalWS;
layout(location = 2) in mat3 PS_IN_tangentToWorld;
layout(location = 3) in vec4 PS_IN_PositionWS;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id2, _streams.normalWS_id2) > 0.0)
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
    _streams.matBlend_id36 = 0.0;
}

void o6S244C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S244C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id5 = vec3(0.0, 0.0, 1.0);
    _streams.matColorBase_id6 = vec4(0.0);
    _streams.matDiffuse_id7 = vec4(0.0);
    _streams.matDiffuseVisible_id22 = vec3(0.0);
    _streams.matSpecular_id9 = vec3(0.0);
    _streams.matSpecularVisible_id24 = vec3(0.0);
    _streams.matSpecularIntensity_id10 = 1.0;
    _streams.matGlossiness_id8 = 0.0;
    _streams.alphaRoughness_id23 = 1.0;
    _streams.matAmbientOcclusion_id11 = 1.0;
    _streams.matAmbientOcclusionDirectLightingFactor_id12 = 0.0;
    _streams.matCavity_id13 = 1.0;
    _streams.matCavityDiffuse_id14 = 0.0;
    _streams.matCavitySpecular_id15 = 0.0;
    _streams.matEmissive_id16 = vec4(0.0);
    _streams.matEmissiveIntensity_id17 = 0.0;
    _streams.matDiffuseSpecularAlphaBlend_id18 = vec2(1.0);
    _streams.matAlphaBlendColor_id19 = vec3(1.0);
    _streams.matAlphaDiscard_id20 = 0.0;
}

void o6S244C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S244C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id27 = 1.0;
}

vec4 o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return PerMaterial_var.o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
}

void o5S244C0_o2S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o5S244C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id7 = colorBase;
    _streams.matColorBase_id6 = colorBase;
}

void o5S244C0_o4S2C0_NormalStream_UpdateNormalFromTangentSpace(inout PS_STREAMS _streams, vec3 normalInTangentSpace)
{
    _streams.normalWS_id2 = normalize(_streams.tangentToWorld_id3 * normalInTangentSpace);
}

void o5S244C0_o4S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
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

float o5S244C0_o4S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(PS_STREAMS _streams, vec3 averageNormal)
{
    float r = length(_streams.matNormal_id5);
    float roughnessAdjust = 0.0;
    if (r < 1.0)
    {
        roughnessAdjust = (0.5 * (1.0 - (r * r))) / ((3.0 * r) - ((r * r) * r));
    }
    return roughnessAdjust;
}

void o5S244C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id35 = mix(1.0, _streams.matAmbientOcclusion_id11, _streams.matAmbientOcclusionDirectLightingFactor_id12);
    _streams.matDiffuseVisible_id22 = ((_streams.matDiffuse_id7.xyz * mix(1.0, _streams.matCavity_id13, _streams.matCavityDiffuse_id14)) * _streams.matDiffuseSpecularAlphaBlend_id18.x) * _streams.matAlphaBlendColor_id19;
    _streams.matSpecularVisible_id24 = (((_streams.matSpecular_id9 * _streams.matSpecularIntensity_id10) * mix(1.0, _streams.matCavity_id13, _streams.matCavitySpecular_id15)) * _streams.matDiffuseSpecularAlphaBlend_id18.y) * _streams.matAlphaBlendColor_id19;
    _streams.NdotV_id25 = max(dot(_streams.normalWS_id2, _streams.viewWS_id21), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id8;
    vec3 param = _streams.matNormal_id5;
    float roughnessAdjust = o5S244C0_o4S2C0_MaterialPixelStream_GetFilterSquareRoughnessAdjustment(_streams, param);
    _streams.alphaRoughness_id23 = max((roughness * roughness) + roughnessAdjust, 0.001000000047497451305389404296875);
}

void o0S447C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id32 = vec3(0.0);
    _streams.envLightSpecularColor_id33 = vec3(0.0);
}

void o0S447C0_LightConstantWhite_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o0S447C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    _streams.envLightDiffuseColor_id32 = vec3(1.0);
    _streams.envLightSpecularColor_id33 = vec3(1.0);
}

vec3 o5S244C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id22;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id24);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id32;
}

void o5S244C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id5;
    o5S244C0_o4S2C0_NormalStream_UpdateNormalFromTangentSpace(_streams, param);
    o5S244C0_o4S2C0_LightStream_ResetLightStream(_streams);
    o5S244C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    vec3 directLightingContribution = vec3(0.0);
    vec3 environmentLightingContribution = vec3(0.0);
    o0S447C0_LightConstantWhite_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o5S244C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id26 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id27 = _streams.matDiffuse_id7.w;
}

void o5S244C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o5S244C0_o2S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o5S244C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id21 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id4.xyz);
    _streams.shadingColor_id26 = vec3(0.0);
    o6S244C1_MaterialPixelShadingStream_ResetStream(_streams);
    o5S244C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

vec4 EffectCompiling_Shading(inout PS_STREAMS _streams)
{
    float factor = (sin(PerFrame_var.Global_Time * 6.0) * 0.25) + 0.25;
    vec4 reloadColor = vec4(0.660000026226043701171875, 1.0, 0.25, 1.0);
    vec4 _112 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    return mix(_112, reloadColor, vec4(factor));
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _12 = EffectCompiling_Shading(_streams);
    _streams.ColorTarget_id1 = _12;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), vec3(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0);
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id2 = PS_IN_normalWS;
    _streams.tangentToWorld_id3 = PS_IN_tangentToWorld;
    _streams.PositionWS_id4 = PS_IN_PositionWS;
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

