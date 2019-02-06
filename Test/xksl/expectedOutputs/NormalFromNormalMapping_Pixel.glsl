#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec3 meshNormal_id0;
    vec3 meshNormalWS_id1;
    vec4 meshTangent_id2;
    mat3 tangentToWorld_id3;
    vec4 ShadingPosition_id4;
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

in vec3 PS_IN_NORMAL;
in vec4 PS_IN_TANGENT;
in vec4 PS_IN_SV_Position;

void NormalUpdate_GenerateNormal_PS()
{
}

mat3 NormalUpdate_GetTangentMatrix(inout PS_STREAMS _streams)
{
    _streams.meshNormal_id0 = normalize(_streams.meshNormal_id0);
    vec3 tangent = normalize(_streams.meshTangent_id2.xyz);
    vec3 bitangent = cross(_streams.meshNormal_id0, tangent) * _streams.meshTangent_id2.w;
    mat3 tangentMatrix = mat3(vec3(tangent), vec3(bitangent), vec3(_streams.meshNormal_id0));
    return tangentMatrix;
}

mat3 NormalFromNormalMapping_GetTangentWorldTransform()
{
    return mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z));
}

void NormalUpdate_UpdateTangentToWorld(inout PS_STREAMS _streams)
{
    mat3 _86 = NormalUpdate_GetTangentMatrix(_streams);
    mat3 tangentMatrix = _86;
    mat3 tangentWorldTransform = NormalFromNormalMapping_GetTangentWorldTransform();
    _streams.tangentToWorld_id3 = tangentWorldTransform * tangentMatrix;
}

void NormalFromNormalMapping_GenerateNormal_PS(inout PS_STREAMS _streams)
{
    NormalUpdate_GenerateNormal_PS();
    NormalUpdate_UpdateTangentToWorld(_streams);
    _streams.meshNormalWS_id1 = mat3(vec3(PerDraw_var.Transformation_WorldInverseTranspose[0].x, PerDraw_var.Transformation_WorldInverseTranspose[0].y, PerDraw_var.Transformation_WorldInverseTranspose[0].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[1].x, PerDraw_var.Transformation_WorldInverseTranspose[1].y, PerDraw_var.Transformation_WorldInverseTranspose[1].z), vec3(PerDraw_var.Transformation_WorldInverseTranspose[2].x, PerDraw_var.Transformation_WorldInverseTranspose[2].y, PerDraw_var.Transformation_WorldInverseTranspose[2].z)) * _streams.meshNormal_id0;
}

void ShaderBase_PSMain()
{
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec3(0.0), vec3(0.0), vec4(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec4(0.0));
    _streams.meshNormal_id0 = PS_IN_NORMAL;
    _streams.meshTangent_id2 = PS_IN_TANGENT;
    _streams.ShadingPosition_id4 = PS_IN_SV_Position;
    NormalFromNormalMapping_GenerateNormal_PS(_streams);
    ShaderBase_PSMain();
}

