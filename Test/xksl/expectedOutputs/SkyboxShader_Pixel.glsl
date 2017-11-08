#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    vec3 skyboxViewDirection_id2;
};

layout(std140) uniform Globals
{
    float SkyboxShader_Intensity;
    layout(row_major) mat4 SkyboxShader_ProjectionInverse;
    layout(row_major) mat4 SkyboxShader_ViewInverse;
    layout(row_major) mat4 SkyboxShader_SkyMatrix;
} Globals_var;

uniform samplerCube SPIRV_Cross_CombinedSkyboxShader_CubeMapTexturing_LinearSampler;

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec3 PS_IN_skyboxViewDirection;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec4 SkyboxShader_Shading(PS_STREAMS _streams)
{
    vec3 samplingDir = normalize(_streams.skyboxViewDirection_id2);
    vec3 color = texture(SPIRV_Cross_CombinedSkyboxShader_CubeMapTexturing_LinearSampler, vec3(samplingDir.x, samplingDir.y, -samplingDir.z)).xyz;
    return vec4(color * Globals_var.SkyboxShader_Intensity, 1.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), vec3(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.skyboxViewDirection_id2 = PS_IN_skyboxViewDirection;
    _streams.ColorTarget_id1 = SkyboxShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

