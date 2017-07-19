#version 450

struct LightDirectional_DirectionalLightData
{
    vec3 DirectionWS;
    vec3 Color;
};

struct VS_STREAMS
{
    float matBlend_id0;
    vec4 ShadingPosition_id1;
    vec3 meshNormal_id2;
    vec4 meshTangent_id3;
    vec3 normalWS_id4;
    vec4 Position_id5;
    vec4 PositionWS_id6;
    float DepthVS_id7;
    vec4 PositionH_id8;
    float matAmbientOcclusion_id9;
    float matAmbientOcclusionDirectLightingFactor_id10;
    float matCavity_id11;
    float matCavityDiffuse_id12;
    float matCavitySpecular_id13;
    vec2 matDiffuseSpecularAlphaBlend_id14;
    vec3 matAlphaBlendColor_id15;
    vec2 TexCoord_id16;
    vec2 TexCoord_id17;
    float matDisplacement_id18;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 Transformation_World;
    layout(row_major) mat4 Transformation_WorldInverse;
    layout(row_major) mat4 Transformation_WorldInverseTranspose;
    layout(row_major) mat4 Transformation_WorldView;
    layout(row_major) mat4 Transformation_WorldViewInverse;
    layout(row_major) mat4 Transformation_WorldViewProjection;
    vec3 Transformation_WorldScale;
    vec4 Transformation_EyeMS;
} PerDraw_var;

layout(std140) uniform PerView
{
    layout(row_major) mat4 Transformation_View;
    layout(row_major) mat4 Transformation_ViewInverse;
    layout(row_major) mat4 Transformation_Projection;
    layout(row_major) mat4 Transformation_ProjectionInverse;
    layout(row_major) mat4 Transformation_ViewProjection;
    vec2 Transformation_ProjScreenRay;
    vec4 Transformation_Eye;
    LightDirectional_DirectionalLightData o0S450C0_LightDirectionalGroup_8__Lights[8];
    int o0S450C0_DirectLightGroupPerView_LightCount;
    vec3 o1S435C0_LightSimpleAmbient_AmbientLight;
} PerView_var;

layout(std140) uniform PerMaterial
{
    float ComputeColorConstantFloatLink_constantFloat;
    vec4 ComputeColorConstantColorLink_constantColor;
    float ComputeColorConstantFloatLink_constantFloat_1;
    vec2 ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 ComputeColorTextureScaledOffsetDynamicSampler_offset;
    float ComputeColorConstantFloatLink_constantFloat_2;
} PerMaterial_var;

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

layout(location = 0) in vec3 VS_IN_meshNormal;
layout(location = 1) in vec4 VS_IN_meshTangent;
layout(location = 2) in vec4 VS_IN_Position;
layout(location = 3) in float VS_IN_matAmbientOcclusion;
layout(location = 4) in float VS_IN_matAmbientOcclusionDirectLightingFactor;
layout(location = 5) in float VS_IN_matCavity;
layout(location = 6) in float VS_IN_matCavityDiffuse;
layout(location = 7) in float VS_IN_matCavitySpecular;
layout(location = 8) in vec2 VS_IN_matDiffuseSpecularAlphaBlend;
layout(location = 9) in vec3 VS_IN_matAlphaBlendColor;
layout(location = 10) in vec2 VS_IN_TexCoord;
layout(location = 11) in vec2 VS_IN_TexCoord_1;
layout(location = 0) out vec4 VS_OUT_ShadingPosition;
layout(location = 1) out vec3 VS_OUT_meshNormal;
layout(location = 2) out vec4 VS_OUT_meshTangent;
layout(location = 3) out vec4 VS_OUT_PositionWS;
layout(location = 4) out float VS_OUT_matAmbientOcclusion;
layout(location = 5) out float VS_OUT_matAmbientOcclusionDirectLightingFactor;
layout(location = 6) out float VS_OUT_matCavity;
layout(location = 7) out float VS_OUT_matCavityDiffuse;
layout(location = 8) out float VS_OUT_matCavitySpecular;
layout(location = 9) out vec2 VS_OUT_matDiffuseSpecularAlphaBlend;
layout(location = 10) out vec3 VS_OUT_matAlphaBlendColor;
layout(location = 11) out vec2 VS_OUT_TexCoord;
layout(location = 12) out vec2 VS_OUT_TexCoord_1;

void ShaderBase_VSMain()
{
}

void o26S2C1_IStreamInitializer_ResetStream()
{
}

void o26S2C1_MaterialStream_ResetStream(out VS_STREAMS _streams)
{
    o26S2C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id0 = 0.0;
}

void o26S2C1_MaterialDisplacementStream_ResetStream(out VS_STREAMS _streams)
{
    o26S2C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id18 = 0.0;
}

vec4 o24S2C0_o22S2C0_o21S2C0_o19S2C0_ComputeColorWave_5_0_01__0_03__Compute(VS_STREAMS _streams)
{
    float phase = length(_streams.TexCoord_id17 - vec2(0.5));
    return vec4(sin((((phase + (PerFrame_var.Global_Time * -0.02999999932944774627685546875)) * 2.0) * 3.1400001049041748046875) * 5.0) * 0.00999999977648258209228515625);
}

vec4 o24S2C0_o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute()
{
    return vec4(PerMaterial_var.ComputeColorConstantFloatLink_constantFloat_2, PerMaterial_var.ComputeColorConstantFloatLink_constantFloat_2, PerMaterial_var.ComputeColorConstantFloatLink_constantFloat_2, PerMaterial_var.ComputeColorConstantFloatLink_constantFloat_2);
}

vec4 o24S2C0_o22S2C0_o21S2C0_ComputeColorMultiply_Compute(VS_STREAMS _streams)
{
    vec4 tex1 = o24S2C0_o22S2C0_o21S2C0_o19S2C0_ComputeColorWave_5_0_01__0_03__Compute(_streams);
    vec4 tex2 = o24S2C0_o22S2C0_o21S2C0_o20S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute();
    vec4 mix1 = tex1 * tex2;
    return mix1;
}

void o24S2C0_o22S2C0_MaterialSurfaceSetStreamFromComputeColor_matDisplacement_r__Compute(inout VS_STREAMS _streams)
{
    _streams.matDisplacement_id18 = o24S2C0_o22S2C0_o21S2C0_ComputeColorMultiply_Compute(_streams).x;
}

void o24S2C0_o23S2C0_MaterialSurfaceDisplacement_Position_meshNormal_false__Compute(inout VS_STREAMS _streams)
{
    vec3 scaledNormal = _streams.meshNormal_id2;
    if (false)
    {
        scaledNormal *= PerDraw_var.Transformation_WorldScale;
    }
    _streams.Position_id5 = vec4(_streams.Position_id5.xyz + (scaledNormal * _streams.matDisplacement_id18), _streams.Position_id5.w);
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
    _streams.PositionWS_id6 = PerDraw_var.Transformation_World * _streams.Position_id5;
}

void TransformationBase_TransformPosition()
{
}

void TransformationBase_PostTransformPosition()
{
}

vec4 TransformationWAndVP_ComputeShadingPosition(vec4 world)
{
    return PerView_var.Transformation_ViewProjection * world;
}

void TransformationWAndVP_PostTransformPosition(inout VS_STREAMS _streams)
{
    TransformationBase_PostTransformPosition();
    vec4 param = _streams.PositionWS_id6;
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
    _streams.normalWS_id4 = vec3(0.0);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, vec4(0.0), vec3(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec4(0.0), 0.0, 0.0, 0.0, 0.0, 0.0, vec2(0.0), vec3(0.0), vec2(0.0), vec2(0.0), 0.0);
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

