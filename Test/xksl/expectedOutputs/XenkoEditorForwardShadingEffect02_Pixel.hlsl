struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float3 meshNormalWS_id2;
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
    float matScatteringStrength_id18;
    float2 matDiffuseSpecularAlphaBlend_id19;
    float3 matAlphaBlendColor_id20;
    float matAlphaDiscard_id21;
    float3 viewWS_id22;
    float3 matDiffuseVisible_id23;
    float alphaRoughness_id24;
    float3 matSpecularVisible_id25;
    float3 shadingColor_id26;
    float shadingColorAlpha_id27;
    float matBlend_id28;
    float2 TexCoord_id29;
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
cbuffer PerMaterial
{
    float4 o12S251C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
    float o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat;
    float o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat;
    float2 o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_scale;
    float2 o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float4 o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_constantColor;
};
SamplerState DynamicSampler_Sampler;
Texture2D<float4> DynamicTexture_Texture;

static float4 PS_IN_ShadingPosition;
static float3 PS_IN_normalWS;
static float4 PS_IN_PositionWS;
static float2 PS_IN_TexCoord;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
    float3 PS_IN_normalWS : NORMALWS;
    float4 PS_IN_PositionWS : POSITION_WS;
    float2 PS_IN_TexCoord : TEXCOORD0;
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
    _streams.matBlend_id28 = 0.0f;
}

void o13S251C1_MaterialPixelStream_ResetStream(inout PS_STREAMS _streams)
{
    o13S251C1_MaterialStream_ResetStream(_streams);
    _streams.matNormal_id5 = float3(0.0f, 0.0f, 1.0f);
    _streams.matColorBase_id6 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuse_id7 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matDiffuseVisible_id23 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecular_id9 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularVisible_id25 = float3(0.0f, 0.0f, 0.0f);
    _streams.matSpecularIntensity_id10 = 1.0f;
    _streams.matGlossiness_id8 = 0.0f;
    _streams.alphaRoughness_id24 = 1.0f;
    _streams.matAmbientOcclusion_id11 = 1.0f;
    _streams.matAmbientOcclusionDirectLightingFactor_id12 = 0.0f;
    _streams.matCavity_id13 = 1.0f;
    _streams.matCavityDiffuse_id14 = 0.0f;
    _streams.matCavitySpecular_id15 = 0.0f;
    _streams.matEmissive_id16 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _streams.matEmissiveIntensity_id17 = 0.0f;
    _streams.matScatteringStrength_id18 = 1.0f;
    _streams.matDiffuseSpecularAlphaBlend_id19 = float2(1.0f, 1.0f);
    _streams.matAlphaBlendColor_id20 = float3(1.0f, 1.0f, 1.0f);
    _streams.matAlphaDiscard_id21 = 0.100000001490116119384765625f;
}

void o13S251C1_MaterialPixelShadingStream_ResetStream(inout PS_STREAMS _streams)
{
    o13S251C1_MaterialPixelStream_ResetStream(_streams);
    _streams.shadingColorAlpha_id27 = 1.0f;
}

float4 o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_GridGizmoBase_GridColorKey__Compute()
{
    return o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_constantColor;
}

float4 o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_Texturing_Texture0_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(PS_STREAMS _streams)
{
    return DynamicTexture_Texture.Sample(DynamicSampler_Sampler, (_streams.TexCoord_id29 * o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_scale) + o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_offset);
}

float4 o12S251C0_o3S2C0_o2S2C0_ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    float4 tex1 = o12S251C0_o3S2C0_o2S2C0_o0S2C0_ComputeColorConstantColorLink_GridGizmoBase_GridColorKey__Compute();
    float4 tex2 = o12S251C0_o3S2C0_o2S2C0_o1S2C1_ComputeColorTextureScaledOffsetDynamicSampler_Texturing_Texture0_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset__Compute(_streams);
    float4 mix1 = tex1 * tex2;
    return mix1;
}

void o12S251C0_o3S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissive_rgba__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissive_id16 = o12S251C0_o3S2C0_o2S2C0_ComputeColorMultiply_Compute(_streams);
}

float4 o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute()
{
    return float4(o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat, o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat, o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat, o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o12S251C0_o5S2C0_MaterialSurfaceSetStreamFromComputeColor_matEmissiveIntensity_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matEmissiveIntensity_id17 = o12S251C0_o5S2C0_o4S2C0_ComputeColorConstantFloatLink_Material_EmissiveIntensity__Compute().x;
}

float4 o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_DiffuseSpecularAlphaBlendValue__Compute()
{
    return float4(o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat, o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_constantFloat);
}

void o12S251C0_o7S2C0_MaterialSurfaceSetStreamFromComputeColor_matDiffuseSpecularAlphaBlend_r__Compute(inout PS_STREAMS _streams)
{
    _streams.matDiffuseSpecularAlphaBlend_id19 = o12S251C0_o7S2C0_o6S2C0_ComputeColorConstantFloatLink_Material_DiffuseSpecularAlphaBlendValue__Compute().x.xx;
}

float4 o12S251C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_Material_AlphaBlendColorValue__Compute()
{
    return o12S251C0_o9S2C0_o8S2C0_ComputeColorConstantColorLink_constantColor;
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
    _streams.shadingColorAlpha_id27 = lerp(0.0f, _streams.shadingColorAlpha_id27, _streams.matDiffuseSpecularAlphaBlend_id19.x);
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

float4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id22 = normalize(Transformation_Eye.xyz - _streams.PositionWS_id4.xyz);
    _streams.shadingColor_id26 = float3(0.0f, 0.0f, 0.0f);
    o13S251C1_MaterialPixelShadingStream_ResetStream(_streams);
    o12S251C0_MaterialSurfaceArray_Compute(_streams);
    return float4(_streams.shadingColor_id26, _streams.shadingColorAlpha_id27);
}

void ShadingBase_PSMain(inout PS_STREAMS _streams)
{
    ShaderBase_PSMain();
    float4 _13 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id1 = _13;
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, float2(0.0f, 0.0f) };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.normalWS_id3 = PS_IN_normalWS;
    _streams.PositionWS_id4 = PS_IN_PositionWS;
    _streams.TexCoord_id29 = PS_IN_TexCoord;
    NormalFromMesh_GenerateNormal_PS(_streams);
    ShadingBase_PSMain(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_normalWS = stage_input.PS_IN_normalWS;
    PS_IN_PositionWS = stage_input.PS_IN_PositionWS;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
