#version 450

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 PositionWS_id1;
    float shadingColorAlpha_id2;
};

layout(location = 0) in vec4 VS_IN_ShadingPosition;
layout(location = 1) in vec4 VS_IN_PositionWS;
layout(location = 2) in float VS_IN_shadingColorAlpha;
layout(location = 0) out vec4 VS_OUT_ShadingPosition;
layout(location = 1) out vec4 VS_OUT_PositionWS;
layout(location = 2) out float VS_OUT_shadingColorAlpha;

void ShaderBase_VSMain()
{
}

void o1S2C1_IStreamInitializer_ResetStream()
{
}

void o0S2C0_IMaterialSurface_Compute()
{
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), 0.0);
    _streams.ShadingPosition_id0 = VS_IN_ShadingPosition;
    _streams.PositionWS_id1 = VS_IN_PositionWS;
    _streams.shadingColorAlpha_id2 = VS_IN_shadingColorAlpha;
    ShaderBase_VSMain();
    o1S2C1_IStreamInitializer_ResetStream();
    o0S2C0_IMaterialSurface_Compute();
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
    VS_OUT_PositionWS = _streams.PositionWS_id1;
    VS_OUT_shadingColorAlpha = _streams.shadingColorAlpha_id2;
}

