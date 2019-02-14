#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    bool IsFrontFace_id1;
    vec4 ColorTarget_id2;
    vec3 meshNormalWS_id3;
    vec3 normalWS_id4;
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
    float matScatteringStrength_id19;
    vec2 matDiffuseSpecularAlphaBlend_id20;
    vec3 matAlphaBlendColor_id21;
    float matAlphaDiscard_id22;
    vec3 viewWS_id23;
    vec3 matDiffuseVisible_id24;
    float alphaRoughness_id25;
    vec3 matSpecularVisible_id26;
    float NdotV_id27;
    vec3 shadingColor_id28;
    float shadingColorAlpha_id29;
    vec3 lightPositionWS_id30;
    vec3 lightDirectionWS_id31;
    vec3 lightColor_id32;
    vec3 lightColorNdotL_id33;
    vec3 envLightDiffuseColor_id34;
    vec3 envLightSpecularColor_id35;
    float NdotL_id36;
    float lightDirectAmbientOcclusion_id37;
    float matBlend_id38;
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
    vec4 o5S251C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
} PerMaterial_var;

in vec4 PS_IN_SV_Position;
in vec3 PS_IN_NORMALWS;
in vec4 PS_IN_POSITION_WS;
in bool PS_IN_SV_IsFrontFace;
out vec4 PS_OUT_ColorTarget;

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id4, _streams.normalWS_id4) > 0.0)
    {
        _streams.normalWS_id4 = normalize(_streams.normalWS_id4);
    }
    _streams.meshNormalWS_id3 = _streams.normalWS_id4;
}

void ShaderBase_PSMain()
{
}

void o6S251C1_IStreamInitializer_ResetStream()
{
}

void o6S251C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S251C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id38 = 0.0;
}

void o6S251C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S251C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id6 = vec3(0.0, 0.0, 1.0);
    _streams.matColorBase_id7 = vec4(0.0);
    _streams.matDiffuse_id8 = vec4(0.0);
    _streams.matDiffuseVisible_id24 = vec3(0.0);
    _streams.matSpecular_id10 = vec3(0.0);
    _streams.matSpecularVisible_id26 = vec3(0.0);
    _streams.matSpecularIntensity_id11 = 1.0;
    _streams.matGlossiness_id9 = 0.0;
    _streams.alphaRoughness_id25 = 1.0;
    _streams.matAmbientOcclusion_id12 = 1.0;
    _streams.matAmbientOcclusionDirectLightingFactor_id13 = 0.0;
    _streams.matCavity_id14 = 1.0;
    _streams.matCavityDiffuse_id15 = 0.0;
    _streams.matCavitySpecular_id16 = 0.0;
    _streams.matEmissive_id17 = vec4(0.0);
    _streams.matEmissiveIntensity_id18 = 0.0;
    _streams.matScatteringStrength_id19 = 1.0;
    _streams.matDiffuseSpecularAlphaBlend_id20 = vec2(1.0);
    _streams.matAlphaBlendColor_id21 = vec3(1.0);
    _streams.matAlphaDiscard_id22 = 0.100000001490116119384765625;
}

void o6S251C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o6S251C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id29 = 1.0;
}

vec4 o5S251C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute()
{
    return PerMaterial_var.o5S251C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_constantColor;
}

void o5S251C0_o2S2C0_MaterialSurfaceDiffuse_Compute(inout PS_STREAMS _streams)
{
    vec4 colorBase = o5S251C0_o2S2C0_o1S2C0_ComputeColorConstantColorLink_Material_DiffuseValue__Compute();
    _streams.matDiffuse_id8 = colorBase;
    _streams.matColorBase_id7 = colorBase;
}

void NormalFromMesh_UpdateNormalFromTangentSpace(vec3 normalInTangentSpace)
{
}

void o5S251C0_o4S2C0_LightStream_ResetLightStream(inout PS_STREAMS _streams)
{
    _streams.lightPositionWS_id30 = vec3(0.0);
    _streams.lightDirectionWS_id31 = vec3(0.0);
    _streams.lightColor_id32 = vec3(0.0);
    _streams.lightColorNdotL_id33 = vec3(0.0);
    _streams.envLightDiffuseColor_id34 = vec3(0.0);
    _streams.envLightSpecularColor_id35 = vec3(0.0);
    _streams.lightDirectAmbientOcclusion_id37 = 1.0;
    _streams.NdotL_id36 = 0.0;
}

void o5S251C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(inout PS_STREAMS _streams)
{
    _streams.lightDirectAmbientOcclusion_id37 = mix(1.0, _streams.matAmbientOcclusion_id12, _streams.matAmbientOcclusionDirectLightingFactor_id13);
    _streams.matDiffuseVisible_id24 = ((_streams.matDiffuse_id8.xyz * mix(1.0, _streams.matCavity_id14, _streams.matCavityDiffuse_id15)) * _streams.matDiffuseSpecularAlphaBlend_id20.x) * _streams.matAlphaBlendColor_id21;
    _streams.matSpecularVisible_id26 = (((_streams.matSpecular_id10 * _streams.matSpecularIntensity_id11) * mix(1.0, _streams.matCavity_id14, _streams.matCavitySpecular_id16)) * _streams.matDiffuseSpecularAlphaBlend_id20.y) * _streams.matAlphaBlendColor_id21;
    _streams.NdotV_id27 = max(dot(_streams.normalWS_id4, _streams.viewWS_id23), 9.9999997473787516355514526367188e-05);
    float roughness = 1.0 - _streams.matGlossiness_id9;
    _streams.alphaRoughness_id25 = max(roughness * roughness, 0.001000000047497451305389404296875);
}

void o5S251C0_o4S2C0_o3S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading()
{
}

void o0S421C0_EnvironmentLight_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    _streams.envLightDiffuseColor_id34 = vec3(0.0);
    _streams.envLightSpecularColor_id35 = vec3(0.0);
}

void o0S421C0_LightConstantWhite_PrepareEnvironmentLight(inout PS_STREAMS _streams)
{
    o0S421C0_EnvironmentLight_PrepareEnvironmentLight(_streams);
    _streams.envLightDiffuseColor_id34 = vec3(1.0);
    _streams.envLightSpecularColor_id35 = vec3(1.0);
}

vec3 o5S251C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(PS_STREAMS _streams)
{
    vec3 diffuseColor = _streams.matDiffuseVisible_id24;
    if (false)
    {
        diffuseColor *= (vec3(1.0) - _streams.matSpecularVisible_id26);
    }
    return diffuseColor * _streams.envLightDiffuseColor_id34;
}

void o5S251C0_o4S2C0_o3S2C0_IMaterialSurfaceShading_AfterLightingAndShading()
{
}

void o5S251C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(inout PS_STREAMS _streams)
{
    vec3 param = _streams.matNormal_id6;
    NormalFromMesh_UpdateNormalFromTangentSpace(param);
    if (!_streams.IsFrontFace_id1)
    {
        _streams.normalWS_id4 = -_streams.normalWS_id4;
    }
    o5S251C0_o4S2C0_LightStream_ResetLightStream(_streams);
    o5S251C0_o4S2C0_MaterialPixelStream_PrepareMaterialForLightingAndShading(_streams);
    o5S251C0_o4S2C0_o3S2C0_IMaterialSurfaceShading_PrepareForLightingAndShading();
    vec3 directLightingContribution = vec3(0.0);
    vec3 environmentLightingContribution = vec3(0.0);
    o0S421C0_LightConstantWhite_PrepareEnvironmentLight(_streams);
    environmentLightingContribution += o5S251C0_o4S2C0_o3S2C0_MaterialSurfaceShadingDiffuseLambert_false__ComputeEnvironmentLightContribution(_streams);
    _streams.shadingColor_id28 += ((directLightingContribution * 3.1415927410125732421875) + environmentLightingContribution);
    _streams.shadingColorAlpha_id29 = _streams.matDiffuse_id8.w;
    o5S251C0_o4S2C0_o3S2C0_IMaterialSurfaceShading_AfterLightingAndShading();
}

void o5S251C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o5S251C0_o2S2C0_MaterialSurfaceDiffuse_Compute(_streams);
    o5S251C0_o4S2C0_MaterialSurfaceLightingAndShading_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id23 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id5.xyz);
    _streams.shadingColor_id28 = vec3(0.0);
    o6S251C1_MaterialPixelShadingStream_ResetStream(_streams);
    o5S251C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id28, _streams.shadingColorAlpha_id29);
}

vec4 EffectCompiling_Shading(inout PS_STREAMS _streams)
{
    float factor = (sin(PerFrame_var.Global_Time * 6.0) * 0.25) + 0.25;
    vec4 reloadColor = vec4(0.660000026226043701171875, 1.0, 0.25, 1.0);
    vec4 _117 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    return mix(_117, reloadColor, vec4(factor));
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _13 = EffectCompiling_Shading(_streams);
    _streams.ColorTarget_id2 = _13;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), false, vec4(0.0), vec3(0.0), vec3(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0);
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.normalWS_id4 = PS_IN_NORMALWS;
    _streams.PositionWS_id5 = PS_IN_POSITION_WS;
    _streams.IsFrontFace_id1 = PS_IN_SV_IsFrontFace;
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

