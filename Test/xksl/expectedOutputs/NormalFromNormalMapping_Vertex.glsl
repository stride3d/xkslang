#version 450

struct VS_STREAMS
{
    vec3 meshNormal_id0;
    vec4 meshTangent_id1;
    vec3 normalWS_id2;
    vec4 ShadingPosition_id3;
};

layout(location = 0) in vec3 VS_IN_meshNormal;
layout(location = 1) in vec4 VS_IN_meshTangent;
layout(location = 2) in vec4 VS_IN_ShadingPosition;
layout(location = 0) out vec3 VS_OUT_meshNormal;
layout(location = 1) out vec4 VS_OUT_meshTangent;
layout(location = 2) out vec4 VS_OUT_ShadingPosition;

void ShaderBase_VSMain()
{
}

void NormalUpdate_GenerateNormal_VS(inout VS_STREAMS _streams)
{
    _streams.normalWS_id2 = vec3(0.0);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec3(0.0), vec4(0.0), vec3(0.0), vec4(0.0));
    _streams.meshNormal_id0 = VS_IN_meshNormal;
    _streams.meshTangent_id1 = VS_IN_meshTangent;
    _streams.ShadingPosition_id3 = VS_IN_ShadingPosition;
    ShaderBase_VSMain();
    NormalUpdate_GenerateNormal_VS(_streams);
    VS_OUT_meshNormal = _streams.meshNormal_id0;
    VS_OUT_meshTangent = _streams.meshTangent_id1;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
}

