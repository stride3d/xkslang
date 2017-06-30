struct PS_STREAMS
{
    float3 meshNormal_id0;
    float4 meshTangent_id1;
    float3x3 tangentToWorld_id2;
    float4 ShadingPosition_id3;
};

cbuffer PerDraw
{
    float4x4 Transformation_WorldInverse;
    float4x4 Transformation_WorldInverseTranspose;
    float4x4 Transformation_WorldView;
    float4x4 Transformation_WorldViewInverse;
    float4x4 Transformation_WorldViewProjection;
    float3 Transformation_WorldScale;
    float4 Transformation_EyeMS;
};

static float3 PS_IN_meshNormal;
static float4 PS_IN_meshTangent;
static float4 PS_IN_ShadingPosition;
static float3x3 PS_OUT_tangentToWorld;

struct SPIRV_Cross_Input
{
    float3 PS_IN_meshNormal : NORMAL;
    float4 PS_IN_meshTangent : TANGENT;
    float4 PS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float3x3 PS_OUT_tangentToWorld : SV_Target0;
};

void NormalBase_GenerateNormal_PS()
{
}

float3x3 NormalStream_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id0 = normalize(_streams.meshNormal_id0);
    float3 tangent = normalize(_streams.meshTangent_id1.xyz);
    float3 bitangent = cross(_streams.meshNormal_id0, tangent) * _streams.meshTangent_id1.w;
    float3x3 tangentMatrix = float3x3(float3(tangent), float3(bitangent), float3(_streams.meshNormal_id0));
    return tangentMatrix;
}

float3x3 NormalFromNormalMapping_GetTangentWorldTransform()
{
    return float3x3(float3(Transformation_WorldInverseTranspose[0].x, Transformation_WorldInverseTranspose[0].y, Transformation_WorldInverseTranspose[0].z), float3(Transformation_WorldInverseTranspose[1].x, Transformation_WorldInverseTranspose[1].y, Transformation_WorldInverseTranspose[1].z), float3(Transformation_WorldInverseTranspose[2].x, Transformation_WorldInverseTranspose[2].y, Transformation_WorldInverseTranspose[2].z));
}

void NormalStream_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    float3x3 _65 = NormalStream_GetTangentMatrix(_streams);
    float3x3 tangentMatrix = _65;
    float3x3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id2 = mul(tangentMatrix, tangentWorldTransform);
}

void NormalFromNormalMapping_GenerateNormal_PS(out PS_STREAMS _streams)
{
    NormalBase_GenerateNormal_PS();
    NormalStream_UpdateTangentToWorld(_streams);
}

void ShaderBase_PSMain()
{
}

void frag_main()
{
    PS_STREAMS _streams = { float3(0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.meshNormal_id0 = PS_IN_meshNormal;
    _streams.meshTangent_id1 = PS_IN_meshTangent;
    _streams.ShadingPosition_id3 = PS_IN_ShadingPosition;
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShaderBase_PSMain();
    PS_OUT_tangentToWorld = _streams.tangentToWorld_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_meshNormal = stage_input.PS_IN_meshNormal;
    PS_IN_meshTangent = stage_input.PS_IN_meshTangent;
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_tangentToWorld = PS_OUT_tangentToWorld;
    return stage_output;
}
