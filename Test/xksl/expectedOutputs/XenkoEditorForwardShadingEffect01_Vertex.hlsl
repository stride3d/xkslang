struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float3 meshNormal_id1;
    float3 normalWS_id2;
    float3x3 tangentToWorld_id3;
    float4 Position_id4;
    float4 PositionWS_id5;
    float DepthVS_id6;
    float4 PositionH_id7;
    float4 ScreenPosition_id8;
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
    float o1S429C0_Camera_NearClipPlane;
    float o1S429C0_Camera_FarClipPlane;
    float2 o1S429C0_Camera_ZProjection;
    float2 o1S429C0_Camera_ViewSize;
    float o1S429C0_Camera_AspectRatio;
    float4 o0S429C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S429C0_LightDirectionalGroup_Lights[8];
    int o0S429C0_DirectLightGroupPerView_LightCount;
    float o1S429C0_LightClustered_ClusterDepthScale;
    float o1S429C0_LightClustered_ClusterDepthBias;
    float2 o1S429C0_LightClustered_ClusterStride;
    float3 o3S414C0_LightSimpleAmbient_AmbientLight;
    float4 o3S414C0_LightSimpleAmbient__padding_PerView_Lighting;
};

static float3 VS_IN_meshNormal;
static float3x3 VS_IN_tangentToWorld;
static float4 VS_IN_Position;
static float4 VS_IN_ScreenPosition;
static float4 VS_OUT_ShadingPosition;
static float3 VS_OUT_normalWS;
static float3x3 VS_OUT_tangentToWorld;
static float4 VS_OUT_PositionWS;
static float4 VS_OUT_ScreenPosition;

struct SPIRV_Cross_Input
{
    float3 VS_IN_meshNormal : NORMAL;
    float3x3 VS_IN_tangentToWorld : TANGENTTOWORLD;
    float4 VS_IN_Position : POSITION;
    float4 VS_IN_ScreenPosition : SCREENPOSITION;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
    float3 VS_OUT_normalWS : NORMALWS;
    float3x3 VS_OUT_tangentToWorld : TANGENTTOWORLD;
    float4 VS_OUT_PositionWS : POSITION_WS;
    float4 VS_OUT_ScreenPosition : SCREENPOSITION;
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
    _streams.PositionWS_id5 = mul(_streams.Position_id4, Transformation_World);
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
    float4 param = _streams.PositionWS_id5;
    _streams.ShadingPosition_id0 = TransformationWAndVP_ComputeShadingPosition(param);
    _streams.PositionH_id7 = _streams.ShadingPosition_id0;
    _streams.DepthVS_id6 = _streams.ShadingPosition_id0.w;
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
    _streams.normalWS_id2 = mul(_streams.meshNormal_id1, float3x3(float3(Transformation_WorldInverseTranspose[0].x, Transformation_WorldInverseTranspose[0].y, Transformation_WorldInverseTranspose[0].z), float3(Transformation_WorldInverseTranspose[1].x, Transformation_WorldInverseTranspose[1].y, Transformation_WorldInverseTranspose[1].z), float3(Transformation_WorldInverseTranspose[2].x, Transformation_WorldInverseTranspose[2].y, Transformation_WorldInverseTranspose[2].z)));
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.meshNormal_id1 = VS_IN_meshNormal;
    _streams.tangentToWorld_id3 = VS_IN_tangentToWorld;
    _streams.Position_id4 = VS_IN_Position;
    _streams.ScreenPosition_id8 = VS_IN_ScreenPosition;
    TransformationBase_VSMain(_streams);
    NormalFromMesh_GenerateNormal_VS(_streams);
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_normalWS = _streams.normalWS_id2;
    VS_OUT_tangentToWorld = _streams.tangentToWorld_id3;
    VS_OUT_PositionWS = _streams.PositionWS_id5;
    VS_OUT_ScreenPosition = _streams.ScreenPosition_id8;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_meshNormal = stage_input.VS_IN_meshNormal;
    VS_IN_tangentToWorld[0] = stage_input.VS_IN_tangentToWorld_0;
    VS_IN_tangentToWorld[1] = stage_input.VS_IN_tangentToWorld_1;
    VS_IN_tangentToWorld[2] = stage_input.VS_IN_tangentToWorld_2;
    VS_IN_Position = stage_input.VS_IN_Position;
    VS_IN_ScreenPosition = stage_input.VS_IN_ScreenPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_normalWS = VS_OUT_normalWS;
    stage_output.VS_OUT_tangentToWorld = VS_OUT_tangentToWorld;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_ScreenPosition = VS_OUT_ScreenPosition;
    return stage_output;
}
