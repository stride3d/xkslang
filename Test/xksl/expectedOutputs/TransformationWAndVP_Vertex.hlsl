struct VS_STREAMS
{
    float4 Position_id0;
    float4 PositionWS_id1;
    float DepthVS_id2;
    float4 ShadingPosition_id3;
    float4 PositionH_id4;
};

cbuffer PerDraw
{
    column_major float4x4 Transformation_World;
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

static float4 VS_IN_Position;
static float4 VS_OUT_PositionWS;
static float VS_OUT_DepthVS;
static float4 VS_OUT_ShadingPosition;
static float4 VS_OUT_PositionH;

struct SPIRV_Cross_Input
{
    float4 VS_IN_Position : POSITION;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_PositionWS : POSITION_WS;
    float VS_OUT_DepthVS : DEPTH_VS;
    float4 VS_OUT_ShadingPosition : SV_Position;
    float4 VS_OUT_PositionH : POSITIONH;
};

void ShaderBase_VSMain()
{
}

void TransformationBase_PreTransformPosition()
{
}

void TransformationWAndVP_PreTransformPosition(inout VS_STREAMS _streams)
{
    TransformationBase_PreTransformPosition();
    _streams.PositionWS_id1 = mul(_streams.Position_id0, Transformation_World);
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
    float4 param = _streams.PositionWS_id1;
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

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xxxx };
    _streams.Position_id0 = VS_IN_Position;
    ShaderBase_VSMain();
    TransformationBase_BaseTransformVS(_streams);
    VS_OUT_PositionWS = _streams.PositionWS_id1;
    VS_OUT_DepthVS = _streams.DepthVS_id2;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
    VS_OUT_PositionH = _streams.PositionH_id4;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_Position = stage_input.VS_IN_Position;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_DepthVS = VS_OUT_DepthVS;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_PositionH = VS_OUT_PositionH;
    return stage_output;
}
