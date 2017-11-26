#version 450

struct VS_STREAMS
{
    vec4 Position_id0;
    vec4 ScreenPosition_id1;
    float ZDepth_id2;
    vec4 ShadingPosition_id3;
    vec2 TexCoord_id4;
    vec4 LocalColor_id5;
};

layout(std140) uniform PerMaterial
{
    vec4 ParticleBase_ColorScale;
    float ParticleBase_AlphaAdditive;
    float ParticleBase_ZOffset;
    float ParticleBase_SoftEdgeInverseDistance;
} PerMaterial_var;

layout(std140) uniform PerView
{
    layout(row_major) mat4 ParticleUtilities_ViewMatrix;
    layout(row_major) mat4 ParticleUtilities_ProjectionMatrix;
    layout(row_major) mat4 ParticleUtilities_ViewProjectionMatrix;
    vec4 ParticleUtilities_ViewFrustum;
    vec4 ParticleUtilities_Viewport;
} PerView_var;

layout(location = 0) in vec4 VS_IN_Position;
layout(location = 1) in vec2 VS_IN_TexCoord;
layout(location = 2) in vec4 VS_IN_LocalColor;
layout(location = 0) out vec4 VS_OUT_ScreenPosition;
layout(location = 1) out float VS_OUT_ZDepth;
layout(location = 2) out vec4 VS_OUT_ShadingPosition;
layout(location = 3) out vec2 VS_OUT_TexCoord;
layout(location = 4) out vec4 VS_OUT_LocalColor;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), 0.0, vec4(0.0), vec2(0.0), vec4(0.0));
    _streams.Position_id0 = VS_IN_Position;
    _streams.TexCoord_id4 = VS_IN_TexCoord;
    _streams.LocalColor_id5 = VS_IN_LocalColor;
    vec4 worldPos = _streams.Position_id0;
    vec4 viewPos = PerView_var.ParticleUtilities_ViewMatrix * worldPos;
    _streams.ShadingPosition_id3 = PerView_var.ParticleUtilities_ProjectionMatrix * viewPos;
    _streams.ScreenPosition_id1 = _streams.ShadingPosition_id3;
    viewPos.w = 1.0;
    viewPos.z += PerMaterial_var.ParticleBase_ZOffset;
    _streams.ZDepth_id2 = viewPos.z;
    vec4 viewProjPos = PerView_var.ParticleUtilities_ProjectionMatrix * viewPos;
    _streams.ShadingPosition_id3.z = (viewProjPos.z / viewProjPos.w) * _streams.ShadingPosition_id3.w;
    VS_OUT_ScreenPosition = _streams.ScreenPosition_id1;
    VS_OUT_ZDepth = _streams.ZDepth_id2;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
    VS_OUT_TexCoord = _streams.TexCoord_id4;
    VS_OUT_LocalColor = _streams.LocalColor_id5;
}

