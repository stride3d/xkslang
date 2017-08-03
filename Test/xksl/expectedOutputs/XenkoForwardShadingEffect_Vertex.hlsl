struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct VS_STREAMS
{
    float4 ShadingPosition_id0;
    float matBlend_id1;
    float3 meshNormal_id2;
    float4 meshTangent_id3;
    float3 normalWS_id4;
    float4 Position_id5;
    float4 PositionWS_id6;
    float DepthVS_id7;
    float4 PositionH_id8;
    float2 TexCoord_id9;
    float2 TexCoord_id10;
    float matDisplacement_id11;
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
    float4 o0S450C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S450C0_LightDirectionalGroup_Lights[8];
    int o0S450C0_DirectLightGroupPerView_LightCount;
    float3 o1S435C0_LightSimpleAmbient_AmbientLight;
    float4 o1S435C0_LightSimpleAmbient__padding_PerView_Lighting;
};

static float3 VS_IN_meshNormal;
static float4 VS_IN_meshTangent;
static float4 VS_IN_Position;
static float2 VS_IN_TexCoord;
static float2 VS_IN_TexCoord_1;
static float4 VS_OUT_ShadingPosition;
static float3 VS_OUT_meshNormal;
static float4 VS_OUT_meshTangent;
static float4 VS_OUT_PositionWS;
static float2 VS_OUT_TexCoord;
static float2 VS_OUT_TexCoord_1;

struct SPIRV_Cross_Input
{
    float3 VS_IN_meshNormal : NORMAL;
    float4 VS_IN_meshTangent : TANGENT;
    float4 VS_IN_Position : POSITION;
    float2 VS_IN_TexCoord : TEXCOORD0;
    float2 VS_IN_TexCoord_1 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
    float3 VS_OUT_meshNormal : NORMAL;
    float4 VS_OUT_meshTangent : TANGENT;
    float4 VS_OUT_PositionWS : POSITION_WS;
    float2 VS_OUT_TexCoord : TEXCOORD0;
    float2 VS_OUT_TexCoord_1 : TEXCOORD0;
};

void ShaderBase_VSMain()
{
}

void o25S34C1_IStreamInitializer_ResetStream()
{
}

void o25S34C1_MaterialStream_ResetStream(out VS_STREAMS _streams)
{
    o25S34C1_IStreamInitializer_ResetStream();
    _streams.matBlend_id1 = 0.0f;
}

void o25S34C1_MaterialDisplacementStream_ResetStream(out VS_STREAMS _streams)
{
    o25S34C1_MaterialStream_ResetStream(_streams);
    _streams.matDisplacement_id11 = 0.0f;
}

void o23S34C0_o22S2C0_MaterialSurfaceDisplacement_Position_meshNormal_false__Compute(inout VS_STREAMS _streams)
{
    float3 scaledNormal = _streams.meshNormal_id2;
    if (false)
    {
        scaledNormal *= Transformation_WorldScale;
    }
    _streams.Position_id5 = float4(_streams.Position_id5.xyz + (scaledNormal * _streams.matDisplacement_id11), _streams.Position_id5.w);
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
    _streams.normalWS_id4 = float3(0.0f, 0.0f, 0.0f);
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, float4(0.0f, 0.0f, 0.0f, 0.0f), float2(0.0f, 0.0f), float2(0.0f, 0.0f), 0.0f };
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

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_meshNormal = stage_input.VS_IN_meshNormal;
    VS_IN_meshTangent = stage_input.VS_IN_meshTangent;
    VS_IN_Position = stage_input.VS_IN_Position;
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    VS_IN_TexCoord_1 = stage_input.VS_IN_TexCoord_1;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_meshNormal = VS_OUT_meshNormal;
    stage_output.VS_OUT_meshTangent = VS_OUT_meshTangent;
    stage_output.VS_OUT_PositionWS = VS_OUT_PositionWS;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    stage_output.VS_OUT_TexCoord_1 = VS_OUT_TexCoord_1;
    return stage_output;
}
