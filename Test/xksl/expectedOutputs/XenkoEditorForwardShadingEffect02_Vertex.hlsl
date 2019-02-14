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
};

static const VS_STREAMS _122 = { 0.0f.xxxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xx };

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
};

static float4 gl_Position;
static float3 VS_IN_NORMAL;
static float4 VS_IN_POSITION;
static float2 VS_IN_TEXCOORD0;
static float3 VS_OUT_normalWS;
static float4 VS_OUT_PositionWS;
static float2 VS_OUT_TexCoord;

struct SPIRV_Cross_Input
{
    float3 VS_IN_NORMAL : NORMAL;
    float4 VS_IN_POSITION : POSITION;
    float2 VS_IN_TEXCOORD0 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_PositionWS : POSITION_WS;
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float3 VS_OUT_normalWS : NORMALWS;
    float4 gl_Position : SV_Position;
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

void vert_main()
{
    VS_STREAMS _streams = _122;
    _streams.meshNormal_id1 = VS_IN_NORMAL;
    _streams.Position_id4 = VS_IN_POSITION;
    _streams.TexCoord_id8 = VS_IN_TEXCOORD0;
    TransformationBase_VSMain(_streams);
    NormalFromMesh_GenerateNormal_VS(_streams);
    gl_Position = _streams.ShadingPosition_id0;
    VS_OUT_normalWS = _streams.normalWS_id3;
    VS_OUT_PositionWS = _streams.PositionWS_id5;
    VS_OUT_TexCoord = _streams.TexCoord_id8;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_NORMAL = stage_input.VS_IN_NORMAL;
    VS_IN_POSITION = stage_input.VS_IN_POSITION;
    VS_IN_TEXCOORD0 = stage_input.VS_IN_TEXCOORD0;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.VS_OUT_normalWS = VS_OUT_normalWS;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
