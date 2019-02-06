#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

in vec4 PS_IN_SV_Position;
in vec3 PS_IN_SKYBOXVIEWDIRECTION;
out vec4 PS_OUT_ColorTarget;

vec4 SkyboxShader_Shading(PS_STREAMS _streams)
{
    vec3 samplingDir = normalize(_streams.skyboxViewDirection_id2);
    vec3 color = texture(SPIRV_Cross_CombinedSkyboxShader_CubeMapTexturing_LinearSampler, vec3(samplingDir.x, samplingDir.y, -samplingDir.z)).xyz;
    return vec4(color * Globals_var.SkyboxShader_Intensity, 1.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), vec3(0.0));
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.skyboxViewDirection_id2 = PS_IN_SKYBOXVIEWDIRECTION;
    _streams.ColorTarget_id1 = SkyboxShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

