#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ScreenPosition_id1;
};

in vec4 PS_IN_SV_Position;
in vec4 PS_IN_SCREENPOSITION;

void o1S2C0_ShaderBase_PSMain()
{
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.ScreenPosition_id1 = PS_IN_SCREENPOSITION;
    _streams.ScreenPosition_id1 /= vec4(_streams.ScreenPosition_id1.w);
    o1S2C0_ShaderBase_PSMain();
}

