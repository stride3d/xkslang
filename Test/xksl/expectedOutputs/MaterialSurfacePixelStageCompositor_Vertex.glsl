#version 450

struct VS_STREAMS
{
    float matAmbientOcclusion_id0;
    float matAmbientOcclusionDirectLightingFactor_id1;
    float matCavity_id2;
    float matCavityDiffuse_id3;
    float matCavitySpecular_id4;
    vec2 matDiffuseSpecularAlphaBlend_id5;
    vec3 matAlphaBlendColor_id6;
    vec4 PositionWS_id7;
    vec4 ShadingPosition_id8;
    mat3 tangentToWorld_id9;
    vec2 TexCoord_id10;
    vec2 TexCoord_id11;
};

layout(location = 0) in float VS_IN_matAmbientOcclusion;
layout(location = 1) in float VS_IN_matAmbientOcclusionDirectLightingFactor;
layout(location = 2) in float VS_IN_matCavity;
layout(location = 3) in float VS_IN_matCavityDiffuse;
layout(location = 4) in float VS_IN_matCavitySpecular;
layout(location = 5) in vec2 VS_IN_matDiffuseSpecularAlphaBlend;
layout(location = 6) in vec3 VS_IN_matAlphaBlendColor;
layout(location = 7) in vec4 VS_IN_PositionWS;
layout(location = 8) in vec4 VS_IN_ShadingPosition;
layout(location = 9) in mat3 VS_IN_tangentToWorld;
layout(location = 10) in vec2 VS_IN_TexCoord;
layout(location = 11) in vec2 VS_IN_TexCoord_1;
layout(location = 0) out float VS_OUT_matAmbientOcclusion;
layout(location = 1) out float VS_OUT_matAmbientOcclusionDirectLightingFactor;
layout(location = 2) out float VS_OUT_matCavity;
layout(location = 3) out float VS_OUT_matCavityDiffuse;
layout(location = 4) out float VS_OUT_matCavitySpecular;
layout(location = 5) out vec2 VS_OUT_matDiffuseSpecularAlphaBlend;
layout(location = 6) out vec3 VS_OUT_matAlphaBlendColor;
layout(location = 7) out vec4 VS_OUT_PositionWS;
layout(location = 8) out vec4 VS_OUT_ShadingPosition;
layout(location = 9) out mat3 VS_OUT_tangentToWorld;
layout(location = 10) out vec2 VS_OUT_TexCoord;
layout(location = 11) out vec2 VS_OUT_TexCoord_1;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, 0.0, 0.0, 0.0, 0.0, vec2(0.0), vec3(0.0), vec4(0.0), vec4(0.0), mat3(vec3(0.0), vec3(0.0), vec3(0.0)), vec2(0.0), vec2(0.0));
    _streams.matAmbientOcclusion_id0 = VS_IN_matAmbientOcclusion;
    _streams.matAmbientOcclusionDirectLightingFactor_id1 = VS_IN_matAmbientOcclusionDirectLightingFactor;
    _streams.matCavity_id2 = VS_IN_matCavity;
    _streams.matCavityDiffuse_id3 = VS_IN_matCavityDiffuse;
    _streams.matCavitySpecular_id4 = VS_IN_matCavitySpecular;
    _streams.matDiffuseSpecularAlphaBlend_id5 = VS_IN_matDiffuseSpecularAlphaBlend;
    _streams.matAlphaBlendColor_id6 = VS_IN_matAlphaBlendColor;
    _streams.PositionWS_id7 = VS_IN_PositionWS;
    _streams.ShadingPosition_id8 = VS_IN_ShadingPosition;
    _streams.tangentToWorld_id9 = VS_IN_tangentToWorld;
    _streams.TexCoord_id10 = VS_IN_TexCoord;
    _streams.TexCoord_id11 = VS_IN_TexCoord_1;
    VS_OUT_matAmbientOcclusion = _streams.matAmbientOcclusion_id0;
    VS_OUT_matAmbientOcclusionDirectLightingFactor = _streams.matAmbientOcclusionDirectLightingFactor_id1;
    VS_OUT_matCavity = _streams.matCavity_id2;
    VS_OUT_matCavityDiffuse = _streams.matCavityDiffuse_id3;
    VS_OUT_matCavitySpecular = _streams.matCavitySpecular_id4;
    VS_OUT_matDiffuseSpecularAlphaBlend = _streams.matDiffuseSpecularAlphaBlend_id5;
    VS_OUT_matAlphaBlendColor = _streams.matAlphaBlendColor_id6;
    VS_OUT_PositionWS = _streams.PositionWS_id7;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id8;
    VS_OUT_tangentToWorld = _streams.tangentToWorld_id9;
    VS_OUT_TexCoord = _streams.TexCoord_id10;
    VS_OUT_TexCoord_1 = _streams.TexCoord_id11;
}

