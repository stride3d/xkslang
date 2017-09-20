#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ScreenPosition_id1;
};

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 1) in vec4 PS_IN_ScreenPosition;

void o1S2C0_ShaderBase_PSMain()
{
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.ScreenPosition_id1 = PS_IN_ScreenPosition;
    _streams.ScreenPosition_id1 /= vec4(_streams.ScreenPosition_id1.w);
    o1S2C0_ShaderBase_PSMain();
}

