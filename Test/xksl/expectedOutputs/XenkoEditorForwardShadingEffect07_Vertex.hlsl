struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float3 meshNormal_id1;
    float3 meshNormalWS_id2;
    float3 normalWS_id3;
    float4 Position_id4;
    float4 PositionWS_id5;
    float DepthVS_id6;
    float4 PositionH_id7;
    float2 TexCoord_id8;
    float4 ScreenPosition_id9;
};

cbuffer PerDraw
{
    column_major float4x4 Transformation_World;
    column_major float4x4 Transformation_WorldInverse;
    column_major float4x4 Transformation_WorldInverseTranspose;
    column_major float4x4 Transformation_WorldView;
    column_major float4x4 Transformation_WorldViewInverse;
    column_major float4x4 Transformation_WorldViewProjection;
    float3 Transformation_WorldScale;
    float4 Transformation_EyeMS;
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
    float o1S437C0_Camera_NearClipPlane;
    float o1S437C0_Camera_FarClipPlane;
    float2 o1S437C0_Camera_ZProjection;
    float2 o1S437C0_Camera_ViewSize;
    float o1S437C0_Camera_AspectRatio;
    float4 o0S437C0_ShadowMapReceiverDirectional__padding_PerView_Default;
    float o0S437C0_ShadowMapReceiverDirectional_CascadeDepthSplits[4];
    LightDirectional_DirectionalLightData o0S437C0_LightDirectionalGroup_Lights[1];
    int o0S437C0_DirectLightGroupPerView_LightCount;
    float o1S437C0_LightClustered_ClusterDepthScale;
    float o1S437C0_LightClustered_ClusterDepthBias;
    float2 o1S437C0_LightClustered_ClusterStride;
    float3 o3S421C0_LightSimpleAmbient_AmbientLight;
    column_major float4x4 o6S421C0_LightSkyboxShader_SkyMatrix;
    float o6S421C0_LightSkyboxShader_Intensity;
    float4 o6S421C0_LightSkyboxShader__padding_PerView_Lighting;
};

static float3 VS_IN_meshNormal;
static float4 VS_IN_Position;
static float2 VS_IN_TexCoord;
static float4 VS_OUT_ShadingPosition;
static float3 VS_OUT_normalWS;
static float4 VS_OUT_PositionWS;
static float VS_OUT_DepthVS;
static float2 VS_OUT_TexCoord;
static float4 VS_OUT_ScreenPosition;

struct SPIRV_Cross_Input
{
    float3 VS_IN_meshNormal : NORMAL;
    float4 VS_IN_Position : POSITION;
    float2 VS_IN_TexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
    float3 VS_OUT_normalWS : NORMALWS;
    float4 VS_OUT_PositionWS : POSITION_WS;
    float VS_OUT_DepthVS : DEPTH_VS;
    float2 VS_OUT_TexCoord : TEXCOORD0;
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
    _streams.meshNormalWS_id2 = mul(_streams.meshNormal_id1, float3x3(float3(Transformation_WorldInverseTranspose[0].x, Transformation_WorldInverseTranspose[0].y, Transformation_WorldInverseTranspose[0].z), float3(Transformation_WorldInverseTranspose[1].x, Transformation_WorldInverseTranspose[1].y, Transformation_WorldInverseTranspose[1].z), float3(Transformation_WorldInverseTranspose[2].x, Transformation_WorldInverseTranspose[2].y, Transformation_WorldInverseTranspose[2].z)));
    _streams.normalWS_id3 = _streams.meshNormalWS_id2;
}

void NormalBase_VSMain(inout VS_STREAMS _streams)
{
    TransformationBase_VSMain(_streams);
    NormalFromMesh_GenerateNormal_VS(_streams);
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx };
    _streams.meshNormal_id1 = VS_IN_meshNormal;
    _streams.Position_id4 = VS_IN_Position;
    _streams.TexCoord_id8 = VS_IN_TexCoord;
    NormalBase_VSMain(_streams);
    _streams.ScreenPosition_id9 = _streams.ShadingPosition_id0;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_normalWS = _streams.normalWS_id3;
    VS_OUT_PositionWS = _streams.PositionWS_id5;
    VS_OUT_DepthVS = _streams.DepthVS_id6;
    VS_OUT_TexCoord = _streams.TexCoord_id8;
    VS_OUT_ScreenPosition = _streams.ScreenPosition_id9;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_meshNormal = stage_input.VS_IN_meshNormal;
    VS_IN_Position = stage_input.VS_IN_Position;
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_normalWS = VS_OUT_normalWS;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_DepthVS = VS_OUT_DepthVS;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    stage_output.VS_OUT_ScreenPosition = VS_OUT_ScreenPosition;
    return stage_output;
}
