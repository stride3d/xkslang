#version 450

struct VS_STREAMS
{
    vec4 Position_id0;
    vec4 PositionWS_id1;
    float DepthVS_id2;
    vec4 ShadingPosition_id3;
    vec4 PositionH_id4;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 Transformation_World;
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

layout(location = 0) in vec4 VS_IN_Position;
layout(location = 0) out vec4 VS_OUT_PositionWS;
layout(location = 1) out float VS_OUT_DepthVS;
layout(location = 2) out vec4 VS_OUT_ShadingPosition;
layout(location = 3) out vec4 VS_OUT_PositionH;

void ShaderBase_VSMain()
{
}

void TransformationBase_PreTransformPosition()
{
}

void TransformationWAndVP_PreTransformPosition(inout VS_STREAMS _streams)
{
    TransformationBase_PreTransformPosition();
    _streams.PositionWS_id1 = PerDraw_var.Transformation_World * _streams.Position_id0;
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
    vec4 param = _streams.PositionWS_id1;
    _streams.ShadingPosition_id3 = TransformationWAndVP_ComputeShadingPosition(param);
    _streams.PositionH_id4 = _streams.ShadingPosition_id3;
    _streams.DepthVS_id2 = _streams.ShadingPosition_id3.w;
}

void TransformationBase_BaseTransformVS(inout VS_STREAMS _streams)
{
    TransformationWAndVP_PreTransformPosition(_streams);
    TransformationBase_TransformPosition();
    TransformationWAndVP_PostTransformPosition(_streams);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), 0.0, vec4(0.0), vec4(0.0));
    _streams.Position_id0 = VS_IN_Position;
    ShaderBase_VSMain();
    TransformationBase_BaseTransformVS(_streams);
    VS_OUT_PositionWS = _streams.PositionWS_id1;
    VS_OUT_DepthVS = _streams.DepthVS_id2;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
    VS_OUT_PositionH = _streams.PositionH_id4;
}

