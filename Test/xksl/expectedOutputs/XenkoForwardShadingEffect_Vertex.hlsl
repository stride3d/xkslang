struct VS_STREAMS
{
    float matBlend_id0;
    float4 ShadingPosition_id1;
    float3 meshNormal_id2;
    float4 meshTangent_id3;
    float3 normalWS_id4;
    float4 Position_id5;
    float4 PositionWS_id6;
    float DepthVS_id7;
    float4 PositionH_id8;
    float matAmbientOcclusion_id9;
    float matAmbientOcclusionDirectLightingFactor_id10;
    float matCavity_id11;
    float matCavityDiffuse_id12;
    float matCavitySpecular_id13;
    float2 matDiffuseSpecularAlphaBlend_id14;
    float3 matAlphaBlendColor_id15;
    float2 TexCoord_id16;
    float2 TexCoord_id17;
    float matDisplacement_id18;
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

static float3 VS_IN_meshNormal;
static float4 VS_IN_meshTangent;
static float4 VS_IN_Position;
static float VS_IN_matAmbientOcclusion;
static float VS_IN_matAmbientOcclusionDirectLightingFactor;
static float VS_IN_matCavity;
static float VS_IN_matCavityDiffuse;
static float VS_IN_matCavitySpecular;
static float2 VS_IN_matDiffuseSpecularAlphaBlend;
static float3 VS_IN_matAlphaBlendColor;
static float2 VS_IN_TexCoord;
static float2 VS_IN_TexCoord_1;
static float4 VS_OUT_ShadingPosition;
static float3 VS_OUT_meshNormal;
static float4 VS_OUT_meshTangent;
static float4 VS_OUT_PositionWS;
static float VS_OUT_matAmbientOcclusion;
static float VS_OUT_matAmbientOcclusionDirectLightingFactor;
static float VS_OUT_matCavity;
static float VS_OUT_matCavityDiffuse;
static float VS_OUT_matCavitySpecular;
static float2 VS_OUT_matDiffuseSpecularAlphaBlend;
static float3 VS_OUT_matAlphaBlendColor;
static float2 VS_OUT_TexCoord;
static float2 VS_OUT_TexCoord_1;

struct SPIRV_Cross_Input
{
    float3 VS_IN_meshNormal : NORMAL;
    float4 VS_IN_meshTangent : TANGENT;
    float4 VS_IN_Position : POSITION;
    float VS_IN_matAmbientOcclusion : TEXCOORD3;
    float VS_IN_matAmbientOcclusionDirectLightingFactor : TEXCOORD4;
    float VS_IN_matCavity : TEXCOORD5;
    float VS_IN_matCavityDiffuse : TEXCOORD6;
    float VS_IN_matCavitySpecular : TEXCOORD7;
    float2 VS_IN_matDiffuseSpecularAlphaBlend : TEXCOORD8;
    float3 VS_IN_matAlphaBlendColor : TEXCOORD9;
    float2 VS_IN_TexCoord : TEXCOORD0;
    float2 VS_IN_TexCoord_1 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
    float3 VS_OUT_meshNormal : NORMAL;
    float4 VS_OUT_meshTangent : TANGENT;
    float4 VS_OUT_PositionWS : POSITION_WS;
    float VS_OUT_matAmbientOcclusion : TEXCOORD4;
    float VS_OUT_matAmbientOcclusionDirectLightingFactor : TEXCOORD5;
    float VS_OUT_matCavity : TEXCOORD6;
    float VS_OUT_matCavityDiffuse : TEXCOORD7;
    float VS_OUT_matCavitySpecular : TEXCOORD8;
    float2 VS_OUT_matDiffuseSpecularAlphaBlend : TEXCOORD9;
    float3 VS_OUT_matAlphaBlendColor : TEXCOORD10;
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float2 VS_OUT_TexCoord_1 : TEXCOORD0;
};

void ShaderBase_VSMain()
{
}

void o26S2C1_IStreamInitializer_ResetStream()
{
}

void o26S2C1_MaterialStream_ResetStream(out VS_STREAMS _streams)
{
    o26S2C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id0 = 0.0f;
}

void o26S2C1_MaterialDisplacementStream_ResetStream(out VS_STREAMS _streams)
{
    o26S2C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id18 = 0.0f;
}

float4 o24S2C0_o22S2C0_o21S2C0_o19S2C0_ComputeColorWave_5_0_01__0_03__Compute(VS_STREAMS _streams)
{
    float phase = length(_streams.TexCoord_id17 - float2(0.5f, 0.5f));
    float _181 = sin((((phase + (Global_Time * -0.02999999932944774627685546875f)) * 2.0f) * 3.1400001049041748046875f) * 5.0f) * 0.00999999977648258209228515625f;
    return float4(_181, _181, _181, _181);
}

float4 o24S2C0_o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute()
{
    return float4(o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__constantFloat, o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__constantFloat, o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__constantFloat, o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__constantFloat);
}

float4 o24S2C0_o22S2C0_o21S2C0_ComputeColorMultiply_Compute(VS_STREAMS _streams)
{
    float4 tex1 = o24S2C0_o22S2C0_o21S2C0_o19S2C0_ComputeColorWave_5_0_01__0_03__Compute(_streams);
    float4 tex2 = o24S2C0_o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute();
    float4 mix1 = tex1 * tex2;
    return mix1;
}

void o24S2C0_o22S2C0_MaterialSurfaceSetStreamFromComputeColor_matDisplacement_r__Compute(inout VS_STREAMS _streams)
{
    _streams.matDisplacement_id18 = o24S2C0_o22S2C0_o21S2C0_ComputeColorMultiply_Compute(_streams).x;
}

void o24S2C0_o23S2C0_MaterialSurfaceDisplacement_Position_meshNormal_false__Compute(inout VS_STREAMS _streams)
{
    float3 scaledNormal = _streams.meshNormal_id2;
    if (false)
    {
        scaledNormal *= o23S2C0_Transformation_WorldScale;
    }
    _streams.Position_id5 = float4(_streams.Position_id5.xyz + (scaledNormal * _streams.matDisplacement_id18), _streams.Position_id5.w);
}

void o24S2C0_MaterialSurfaceArray_Compute(out VS_STREAMS _streams)
{
    o24S2C0_o22S2C0_MaterialSurfaceSetStreamFromComputeColor_matDisplacement_r__Compute(_streams);
    o24S2C0_o23S2C0_MaterialSurfaceDisplacement_Position_meshNormal_false__Compute(_streams);
}

void MaterialSurfaceVertexStageCompositor_VSMain(out VS_STREAMS _streams)
{
    ShaderBase_VSMain();
    o26S2C1_MaterialDisplacementStream_ResetStream(_streams);
    o24S2C0_MaterialSurfaceArray_Compute(_streams);
}

void TransformationBase_PreTransformPosition()
{
}

void TransformationWAndVP_PreTransformPosition(inout VS_STREAMS _streams)
{
    TransformationBase_PreTransformPosition();
    _streams.PositionWS_id6 = mul(_streams.Position_id5, Transformation_World);
}

void TransformationBase_TransformPosition()
{
}

void TransformationBase_PostTransformPosition()
{
}

float4 TransformationWAndVP_ComputeShadingPosition(float4 world)
{
    return mul(world, Transformation_ViewProjection);
}

void TransformationWAndVP_PostTransformPosition(inout VS_STREAMS _streams)
{
    TransformationBase_PostTransformPosition();
    float4 param = _streams.PositionWS_id6;
    _streams.ShadingPosition_id1 = TransformationWAndVP_ComputeShadingPosition(param);
    _streams.PositionH_id8 = _streams.ShadingPosition_id1;
    _streams.DepthVS_id7 = _streams.ShadingPosition_id1.w;
}

void TransformationBase_BaseTransformVS(out VS_STREAMS _streams)
{
    TransformationWAndVP_PreTransformPosition(_streams);
    TransformationBase_TransformPosition();
    TransformationWAndVP_PostTransformPosition(_streams);
}

void TransformationBase_VSMain(out VS_STREAMS _streams)
{
    MaterialSurfaceVertexStageCompositor_VSMain(_streams);
    TransformationBase_BaseTransformVS(_streams);
}

void NormalBase_GenerateNormal_VS(out VS_STREAMS _streams)
{
    _streams.normalWS_id4 = float3(0.0f, 0.0f, 0.0f);
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float2(0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float2(0.0f, 0.0f), float2(0.0f, 0.0f), 0.0f };
    _streams.meshNormal_id2 = VS_IN_meshNormal;
    _streams.meshTangent_id3 = VS_IN_meshTangent;
    _streams.Position_id5 = VS_IN_Position;
    _streams.matAmbientOcclusion_id9 = VS_IN_matAmbientOcclusion;
    _streams.matAmbientOcclusionDirectLightingFactor_id10 = VS_IN_matAmbientOcclusionDirectLightingFactor;
    _streams.matCavity_id11 = VS_IN_matCavity;
    _streams.matCavityDiffuse_id12 = VS_IN_matCavityDiffuse;
    _streams.matCavitySpecular_id13 = VS_IN_matCavitySpecular;
    _streams.matDiffuseSpecularAlphaBlend_id14 = VS_IN_matDiffuseSpecularAlphaBlend;
    _streams.matAlphaBlendColor_id15 = VS_IN_matAlphaBlendColor;
    _streams.TexCoord_id16 = VS_IN_TexCoord;
    _streams.TexCoord_id17 = VS_IN_TexCoord_1;
    TransformationBase_VSMain(_streams);
    NormalBase_GenerateNormal_VS(_streams);
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id1;
    VS_OUT_meshNormal = _streams.meshNormal_id2;
    VS_OUT_meshTangent = _streams.meshTangent_id3;
    VS_OUT_PositionWS = _streams.PositionWS_id6;
    VS_OUT_matAmbientOcclusion = _streams.matAmbientOcclusion_id9;
    VS_OUT_matAmbientOcclusionDirectLightingFactor = _streams.matAmbientOcclusionDirectLightingFactor_id10;
    VS_OUT_matCavity = _streams.matCavity_id11;
    VS_OUT_matCavityDiffuse = _streams.matCavityDiffuse_id12;
    VS_OUT_matCavitySpecular = _streams.matCavitySpecular_id13;
    VS_OUT_matDiffuseSpecularAlphaBlend = _streams.matDiffuseSpecularAlphaBlend_id14;
    VS_OUT_matAlphaBlendColor = _streams.matAlphaBlendColor_id15;
    VS_OUT_TexCoord = _streams.TexCoord_id16;
    VS_OUT_TexCoord_1 = _streams.TexCoord_id17;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_meshNormal = stage_input.VS_IN_meshNormal;
    VS_IN_meshTangent = stage_input.VS_IN_meshTangent;
    VS_IN_Position = stage_input.VS_IN_Position;
    VS_IN_matAmbientOcclusion = stage_input.VS_IN_matAmbientOcclusion;
    VS_IN_matAmbientOcclusionDirectLightingFactor = stage_input.VS_IN_matAmbientOcclusionDirectLightingFactor;
    VS_IN_matCavity = stage_input.VS_IN_matCavity;
    VS_IN_matCavityDiffuse = stage_input.VS_IN_matCavityDiffuse;
    VS_IN_matCavitySpecular = stage_input.VS_IN_matCavitySpecular;
    VS_IN_matDiffuseSpecularAlphaBlend = stage_input.VS_IN_matDiffuseSpecularAlphaBlend;
    VS_IN_matAlphaBlendColor = stage_input.VS_IN_matAlphaBlendColor;
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    VS_IN_TexCoord_1 = stage_input.VS_IN_TexCoord_1;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_meshNormal = VS_OUT_meshNormal;
    stage_output.VS_OUT_meshTangent = VS_OUT_meshTangent;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_matAmbientOcclusion = VS_OUT_matAmbientOcclusion;
    stage_output.VS_OUT_matAmbientOcclusionDirectLightingFactor = VS_OUT_matAmbientOcclusionDirectLightingFactor;
    stage_output.VS_OUT_matCavity = VS_OUT_matCavity;
    stage_output.VS_OUT_matCavityDiffuse = VS_OUT_matCavityDiffuse;
    stage_output.VS_OUT_matCavitySpecular = VS_OUT_matCavitySpecular;
    stage_output.VS_OUT_matDiffuseSpecularAlphaBlend = VS_OUT_matDiffuseSpecularAlphaBlend;
    stage_output.VS_OUT_matAlphaBlendColor = VS_OUT_matAlphaBlendColor;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    stage_output.VS_OUT_TexCoord_1 = VS_OUT_TexCoord_1;
    return stage_output;
}
