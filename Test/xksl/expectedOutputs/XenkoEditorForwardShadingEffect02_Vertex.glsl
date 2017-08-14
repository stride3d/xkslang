#version 450

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec3 meshNormal_id1;
    vec3 normalWS_id2;
    vec4 Position_id3;
    vec4 PositionWS_id4;
    float DepthVS_id5;
    vec4 PositionH_id6;
    vec2 TexCoord_id7;
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
} PerView_var;

layout(location = 0) in vec3 VS_IN_meshNormal;
layout(location = 1) in vec4 VS_IN_Position;
layout(location = 2) in vec2 VS_IN_TexCoord;
layout(location = 0) out vec4 VS_OUT_ShadingPosition;
layout(location = 1) out vec3 VS_OUT_normalWS;
layout(location = 2) out vec4 VS_OUT_PositionWS;
layout(location = 3) out vec2 VS_OUT_TexCoord;

void ShaderBase_VSMain()
{
}

void TransformationBase_PreTransformPosition()
{
}

void TransformationWAndVP_PreTransformPosition(inout VS_STREAMS _streams)
{
    TransformationBase_PreTransformPosition();
    _streams.PositionWS_id4 = PerDraw_var.Transformation_World * _streams.Position_id3;
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
    vec4 param = _streams.PositionWS_id4;
    _streams.ShadingPosition_id0 = TransformationWAndVP_ComputeShadingPosition(param);
    _streams.PositionH_id6 = _streams.ShadingPosition_id0;
    _streams.DepthVS_id5 = _streams.ShadingPosition_id0.w;
}

void TransformationBase_BaseTransformVS(inout VS_STREAMS _streams)
{
    TransformationWAndVP_PreTransformPosition(_streams);
    TransformationBase_TransformPosition();
    TransformationWAndVP_PostTransformPosition(_streams);
}

void TransformationBase_VSMain(inout VS_STREAMS _streams)
{
    ShaderBase_VSMain();
    TransformationBase_BaseTransformVS(_streams);
}

void NormalFromMesh_GenerateNormal_VS(inout VS_STREAMS _streams)
{
    _streams.normalWS_id2 = mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z)) * _streams.meshNormal_id1;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec3(0.0), vec3(0.0), vec4(0.0), vec4(0.0), 0.0, vec4(0.0), vec2(0.0));
    _streams.meshNormal_id1 = VS_IN_meshNormal;
    _streams.Position_id3 = VS_IN_Position;
    _streams.TexCoord_id7 = VS_IN_TexCoord;
    TransformationBase_VSMain(_streams);
    NormalFromMesh_GenerateNormal_VS(_streams);
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_normalWS = _streams.normalWS_id2;
    VS_OUT_PositionWS = _streams.PositionWS_id4;
    VS_OUT_TexCoord = _streams.TexCoord_id7;
}

