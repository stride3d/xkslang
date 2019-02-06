#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
};

in vec4 PS_IN_SV_Position;
out vec4 PS_OUT_ColorTarget;

void ShaderBase_PSMain()
{
}

vec4 o0S27C0_ComputeColor_Compute()
{
    return vec4(0.0);
}

vec4 ShadingBase_Shading()
{
    return o0S27C0_ComputeColor_Compute();
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    ShaderBase_PSMain();
    _streams.ColorTarget_id1 = ShadingBase_Shading();
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

