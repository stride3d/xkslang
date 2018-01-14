#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    vec3 meshNormalWS_id2;
    vec3 normalWS_id3;
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
    float matScatteringStrength_id18;
    vec2 matDiffuseSpecularAlphaBlend_id19;
    vec3 matAlphaBlendColor_id20;
    float matAlphaDiscard_id21;
    vec3 viewWS_id22;
    vec3 matDiffuseVisible_id23;
    float alphaRoughness_id24;
    vec3 matSpecularVisible_id25;
    vec3 shadingColor_id26;
    float shadingColorAlpha_id27;
    float matBlend_id28;
    vec2 TexCoord_id29;
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

layout(std140) uniform PerMaterial
{
    vec4 o12S251C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
    float o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat;
    vec2 o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    vec4 o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_constantColor;
} PerMaterial_var;

uniform sampler2D SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_normalWS;
layout(location = 2) in vec4 PS_IN_PositionWS;
layout(location = 3) in vec2 PS_IN_TexCoord;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void NormalFromMesh_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    if (dot(_streams.normalWS_id3, _streams.normalWS_id3) > 0.0)
    {
        _streams.normalWS_id3 = normalize(_streams.normalWS_id3);
    }
    _streams.meshNormalWS_id2 = _streams.normalWS_id3;
}

void ShaderBase_PSMain()
{
}

void o13S251C1_IStreamInitializer_ResetStream()
{
}

void o13S251C1_MaterialStream_ResetStream(inout PS_STREAMS _streams)
{
    o13S251C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id28 = 0.0;
}

void o13S251C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o13S251C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id5 = vec3(0.0, 0.0, 1.0);
    _streams.matColorBase_id6 = vec4(0.0);
    _streams.matDiffuse_id7 = vec4(0.0);
    _streams.matDiffuseVisible_id23 = vec3(0.0);
    _streams.matSpecular_id9 = vec3(0.0);
    _streams.matSpecularVisible_id25 = vec3(0.0);
    _streams.matSpecularIntensity_id10 = 1.0;
    _streams.matGlossiness_id8 = 0.0;
    _streams.alphaRoughness_id24 = 1.0;
    _streams.matAmbientOcclusion_id11 = 1.0;
    _streams.matAmbientOcclusionDirectLightingFactor_id12 = 0.0;
    _streams.matCavity_id13 = 1.0;
    _streams.matCavityDiffuse_id14 = 0.0;
    _streams.matCavitySpecular_id15 = 0.0;
    _streams.matEmissive_id16 = vec4(0.0);
    _streams.matEmissiveIntensity_id17 = 0.0;
    _streams.matScatteringStrength_id18 = 1.0;
    _streams.matDiffuseSpecularAlphaBlend_id19 = vec2(1.0);
    _streams.matAlphaBlendColor_id20 = vec3(1.0);
    _streams.matAlphaDiscard_id21 = 0.100000001490116119384765625;
}

void o13S251C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o13S251C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id27 = 1.0;
}

vec4 o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_GridGizmoBase_GridColorKey__Compute()
{
    return PerMaterial_var.o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_constantColor;
}

vec4 o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_Texturing_Texture0_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedDynamicTexture_TextureDynamicSampler_Sampler, (_streams.TexCoord_id29 * PerMaterial_var.o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

vec4 o12S251C0_o3S2C0_o2S2C0_ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    vec4 tex1 = o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_GridGizmoBase_GridColorKey__Compute();
    vec4 tex2 = o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_Texturing_Texture0_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    vec4 mix1 = tex1 * tex2;
    return mix1;
}

void o12S251C0_o3S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissive_rgba__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissive_id16 = o12S251C0_o3S2C0_o2S2C0_ComputeColorMultiply_Compute(_streams);
}

vec4 o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute()
{
    return vec4(PerMaterial_var.o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o12S251C0_o5S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissiveIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissiveIntensity_id17 = o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute().x;
}

vec4 o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_DiffuseSpecularAlphaBlendValue__Compute()
{
    return vec4(PerMaterial_var.o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o12S251C0_o7S2C0_MaterialSurfaceSetStreamFromComputeColor_matDiffuseSpecularAlphaBlend_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matDiffuseSpecularAlphaBlend_id19 = vec2(o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_DiffuseSpecularAlphaBlendValue__Compute().x);
}

vec4 o12S251C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_AlphaBlendColorValue__Compute()
{
    return PerMaterial_var.o12S251C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
}

void o12S251C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matAlphaBlendColor_rgb__Compute(inout PS_STREAMS _streams)
{
    _streams.matAlphaBlendColor_id20 = o12S251C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_AlphaBlendColorValue__Compute().xyz;
}

void o12S251C0_o10S2C0_MaterialSurfaceEmissiveShading_true__Compute(inout PS_STREAMS _streams)
{
    _streams.shadingColor_id26 += (_streams.matEmissive_id16.xyz * _streams.matEmissiveIntensity_id17);
    if (true)
    {
        _streams.shadingColorAlpha_id27 = _streams.matEmissive_id16.w;
    }
}

void o12S251C0_o11S2C0_MaterialSurfaceDiffuseSpecularAlphaBlendColor_Compute(inout PS_STREAMS _streams)
{
    _streams.shadingColorAlpha_id27 = mix(0.0, _streams.shadingColorAlpha_id27, _streams.matDiffuseSpecularAlphaBlend_id19.x);
}

void o12S251C0_MaterialSurfaceArray_Compute(inout PS_STREAMS _streams)
{
    o12S251C0_o3S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissive_rgba__Compute(_streams);
    o12S251C0_o5S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissiveIntensity_r__Compute(_streams);
    o12S251C0_o7S2C0_MaterialSurfaceSetStreamFromComputeColor_matDiffuseSpecularAlphaBlend_r__Compute(_streams);
    o12S251C0_o9S2C0_MaterialSurfaceSetStreamFromComputeColor_matAlphaBlendColor_rgb__Compute(_streams);
    o12S251C0_o10S2C0_MaterialSurfaceEmissiveShading_true__Compute(_streams);
    o12S251C0_o11S2C0_MaterialSurfaceDiffuseSpecularAlphaBlendColor_Compute(_streams);
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id22 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id4.xyz);
    _streams.shadingColor_id26 = vec3(0.0);
    o13S251C1_MaterialPixelShadingStream_ResetStream(_streams);
    o12S251C0_MaterialSurfaceArray_Compute(_streams);
    return vec4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    vec4 _13 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id1 = _13;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), vec3(0.0), vec3(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec3(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, vec4(0.0), 0.0, 0.0, vec2(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, vec3(0.0), vec3(0.0), 0.0, 0.0, vec2(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id3 = PS_IN_normalWS;
    _streams.PositionWS_id4 = PS_IN_PositionWS;
    _streams.TexCoord_id29 = PS_IN_TexCoord;
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

