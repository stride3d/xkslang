#version 450

struct LightDirectional_DirectionalLightData
{
    vec3 DirectionWS;
    vec3 Color;
};

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
    float matBlend_id1;
    vec3 meshNormal_id2;
    vec4 meshTangent_id3;
    vec3 normalWS_id4;
    vec4 Position_id5;
    vec4 PositionWS_id6;
    float DepthVS_id7;
    vec4 PositionH_id8;
    vec2 TexCoord_id9;
    vec2 TexCoord_id10;
    float matDisplacement_id11;
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
    vec4 o0S450C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S450C0_LightDirectionalGroup_Lights[8];
    int o0S450C0_DirectLightGroupPerView_LightCount;
    vec3 o1S435C0_LightSimpleAmbient_AmbientLight;
    vec4 o1S435C0_LightSimpleAmbient__padding_PerView_Lighting;
} PerView_var;

layout(location = 0) in vec3 VS_IN_meshNormal;
layout(location = 1) in vec4 VS_IN_meshTangent;
layout(location = 2) in vec4 VS_IN_Position;
layout(location = 3) in vec2 VS_IN_TexCoord;
layout(location = 4) in vec2 VS_IN_TexCoord_1;
layout(location = 0) out vec4 VS_OUT_ShadingPosition;
layout(location = 1) out vec3 VS_OUT_meshNormal;
layout(location = 2) out vec4 VS_OUT_meshTangent;
layout(location = 3) out vec4 VS_OUT_PositionWS;
layout(location = 4) out vec2 VS_OUT_TexCoord;
layout(location = 5) out vec2 VS_OUT_TexCoord_1;

void ShaderBase_VSMain()
{
}

void o25S34C1_IStreamInitializer_ResetStream()
{
}

void o25S34C1_MaterialStream_ResetStream(out VS_STREAMS _streams)
{
    o25S34C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id1 = 0.0;
}

void o25S34C1_MaterialDisplacementStream_ResetStream(out VS_STREAMS _streams)
{
    o25S34C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id11 = 0.0;
}

void o23S34C0_o22S2C0_MaterialSurfaceDisplacement_Position_meshNormal_false__Compute(inout VS_STREAMS _streams)
{
    vec3 scaledNormal = _streams.meshNormal_id2;
    if (false)
    {
        scaledNormal *= PerDraw_var.Transformation_WorldScale;
    }
    _streams.Position_id5 = vec4(_streams.Position_id5.xyz + (scaledNormal * _streams.matDisplacement_id11), _streams.Position_id5.w);
}

void o23S34C0_MaterialSurfaceArray_Compute(out VS_STREAMS _streams)
{
    o23S34C0_o22S2C0_MaterialSurfaceDisplacement_Position_meshNormal_false__Compute(_streams);
}

void MaterialSurfaceVertexStageCompositor_VSMain(out VS_STREAMS _streams)
{
    ShaderBase_VSMain();
    o25S34C1_MaterialDisplacementStream_ResetStream(_streams);
    o23S34C0_MaterialSurfaceArray_Compute(_streams);
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
    _streams.ShadingPosition_id0 = TransformationWAndVP_ComputeShadingPosition(param);
    _streams.PositionH_id8 = _streams.ShadingPosition_id0;
    _streams.DepthVS_id7 = _streams.ShadingPosition_id0.w;
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
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), 0.0, vec3(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec4(0.0), vec2(0.0), vec2(0.0), 0.0);
    _streams.meshNormal_id2 = VS_IN_meshNormal;
    _streams.meshTangent_id3 = VS_IN_meshTangent;
    _streams.Position_id5 = VS_IN_Position;
    _streams.TexCoord_id9 = VS_IN_TexCoord;
    _streams.TexCoord_id10 = VS_IN_TexCoord_1;
    TransformationBase_VSMain(_streams);
    NormalBase_GenerateNormal_VS(_streams);
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_meshNormal = _streams.meshNormal_id2;
    VS_OUT_meshTangent = _streams.meshTangent_id3;
    VS_OUT_PositionWS = _streams.PositionWS_id6;
    VS_OUT_TexCoord = _streams.TexCoord_id9;
    VS_OUT_TexCoord_1 = _streams.TexCoord_id10;
}

