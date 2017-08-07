#version 450

struct PS_STREAMS
{
    vec3 meshNormal_id0;
    vec4 meshTangent_id1;
    mat3 tangentToWorld_id2;
    vec4 ShadingPosition_id3;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 Transformation_WorldInverse;
    layout(row_major) mat4 Transformation_WorldInverseTranspose;
    layout(row_major) mat4 Transformation_WorldView;
    layout(row_major) mat4 Transformation_WorldViewInverse;
    layout(row_major) mat4 Transformation_WorldViewProjection;
    vec3 Transformation_WorldScale;
    vec4 Transformation_EyeMS;
} PerDraw_var;

layout(location = 0) in vec3 PS_IN_meshNormal;
layout(location = 1) in vec4 PS_IN_meshTangent;
layout(location = 2) in vec4 PS_IN_ShadingPosition;

void NormalBase_GenerateNormal_PS()
{
}

mat3 NormalStream_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id0 = normalize(_streams.meshNormal_id0);
    vec3 tangent = normalize(_streams.meshTangent_id1.xyz);
    vec3 bitangent = cross(_streams.meshNormal_id0, tangent) * _streams.meshTangent_id1.w;
    mat3 tangentMatrix = mat3(vec3(tangent), vec3(bitangent), vec3(_streams.meshNormal_id0));
    return tangentMatrix;
}

mat3 NormalFromNormalMapping_GetTangentWorldTransform()
{
    return mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z));
}

void NormalStream_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    mat3 _65 = NormalStream_GetTangentMatrix(_streams);
    mat3 tangentMatrix = _65;
    mat3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id2 = tangentWorldTransform * tangentMatrix;
}

void NormalFromNormalMapping_GenerateNormal_PS(out PS_STREAMS _streams)
{
    NormalBase_GenerateNormal_PS();
    NormalStream_UpdateTangentToWorld(_streams);
}

void ShaderBase_PSMain()
{
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec3(0.0), vec4(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0));
    _streams.meshNormal_id0 = PS_IN_meshNormal;
    _streams.meshTangent_id1 = PS_IN_meshTangent;
    _streams.ShadingPosition_id3 = PS_IN_ShadingPosition;
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShaderBase_PSMain();
}

