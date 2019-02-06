#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec3 skyboxViewDirection_id1;
    vec4 Position_id2;
};

layout(std140) uniform Globals
{
    float SkyboxShader_Intensity;
    layout(row_major) mat4 SkyboxShader_ProjectionInverse;
    layout(row_major) mat4 SkyboxShader_ViewInverse;
    layout(row_major) mat4 SkyboxShader_SkyMatrix;
} Globals_var;

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 SpriteBase_MatrixTransform;
} PerDraw_var;

in vec4 VS_IN_POSITION;
out vec3 VS_OUT_skyboxViewDirection;

void SpriteBase_VSMain(inout VS_STREAMS _streams)
{
    _streams.ShadingPosition_id0 = PerDraw_var.SpriteBase_MatrixTransform * _streams.Position_id2;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec3(0.0), vec4(0.0));
    _streams.Position_id2 = VS_IN_POSITION;
    SpriteBase_VSMain(_streams);
    vec4 screenPosition = _streams.ShadingPosition_id0 / vec4(_streams.ShadingPosition_id0.w);
    vec4 position = vec4(screenPosition.x, screenPosition.y, 1.0, 1.0);
    vec3 directionVS = (Globals_var.SkyboxShader_ProjectionInverse * position).xyz;
    vec3 directionWS = (Globals_var.SkyboxShader_ViewInverse * vec4(directionVS, 0.0)).xyz;
    _streams.skyboxViewDirection_id1 = mat3(vec3(Globals_var.SkyboxShader_SkyMatrix[0].x, Globals_var.SkyboxShader_SkyMatrix[0].y, Globals_var.SkyboxShader_SkyMatrix[0].z), vec3(Globals_var.SkyboxShader_SkyMatrix[1].x, Globals_var.SkyboxShader_SkyMatrix[1].y, Globals_var.SkyboxShader_SkyMatrix[1].z), vec3(Globals_var.SkyboxShader_SkyMatrix[2].x, Globals_var.SkyboxShader_SkyMatrix[2].y, Globals_var.SkyboxShader_SkyMatrix[2].z)) * directionWS;
    gl_Position = _streams.ShadingPosition_id0;
    VS_OUT_skyboxViewDirection = _streams.skyboxViewDirection_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

