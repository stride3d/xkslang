#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 PositionWS_id1;
    float shadingColorAlpha_id2;
};

in vec4 VS_IN_SV_Position;
in vec4 VS_IN_POSITION_WS;
in float VS_IN_SHADINGCOLORALPHA;
out vec4 VS_OUT_PositionWS;
out float VS_OUT_shadingColorAlpha;

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
    _streams.ShadingPosition_id0 = VS_IN_SV_Position;
    _streams.PositionWS_id1 = VS_IN_POSITION_WS;
    _streams.shadingColorAlpha_id2 = VS_IN_SHADINGCOLORALPHA;
    ShaderBase_VSMain();
    o1S2C1_IStreamInitializer_ResetStream();
    o0S2C0_IMaterialSurface_Compute();
    gl_Position = _streams.ShadingPosition_id0;
    VS_OUT_PositionWS = _streams.PositionWS_id1;
    VS_OUT_shadingColorAlpha = _streams.shadingColorAlpha_id2;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

