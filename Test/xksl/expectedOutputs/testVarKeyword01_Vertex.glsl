#version 450

struct VS_STREAMS
{
    vec4 meshTangent_id0;
};

layout(location = 0) in vec4 VS_IN_meshTangent;

mat3 ShaderTest_GetTangentMatrix()
{
    mat3 tangent = mat3(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));
    return tangent;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.meshTangent_id0 = VS_IN_meshTangent;
    mat3 f3x3 = ShaderTest_GetTangentMatrix();
    vec3 f3 = _streams.meshTangent_id0.xyz;
    int k = 10;
    k = ivec4(_streams.meshTangent_id0).x;
}

