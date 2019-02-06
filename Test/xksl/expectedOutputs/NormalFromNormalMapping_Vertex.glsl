#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec3 meshNormal_id0;
    vec4 meshTangent_id1;
    vec3 normalWS_id2;
    vec4 ShadingPosition_id3;
};

in vec3 VS_IN_NORMAL;
in vec4 VS_IN_TANGENT;
in vec4 VS_IN_SV_Position;
out vec3 VS_OUT_meshNormal;
out vec4 VS_OUT_meshTangent;

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
    _streams.meshNormal_id0 = VS_IN_NORMAL;
    _streams.meshTangent_id1 = VS_IN_TANGENT;
    _streams.ShadingPosition_id3 = VS_IN_SV_Position;
    ShaderBase_VSMain();
    NormalUpdate_GenerateNormal_VS(_streams);
    VS_OUT_meshNormal = _streams.meshNormal_id0;
    VS_OUT_meshTangent = _streams.meshTangent_id1;
    gl_Position = _streams.ShadingPosition_id3;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

