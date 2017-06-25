#version 450

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    vec4 ColorTarget1_id2;
};

layout(location = 0) in vec4 PS_IN_ShadingPosition;
layout(location = 0) out vec4 PS_OUT_ColorTarget;
layout(location = 1) out vec4 PS_OUT_ColorTarget1;

void ShaderBase_PSMain()
{
}

vec4 o0S2C0_ComputeColor_Compute()
{
    return vec4(0.0);
}

vec4 ShadingBase_Shading()
{
    return o0S2C0_ComputeColor_Compute();
}

vec4 o1S2C1_ComputeColor_Compute()
{
    return vec4(0.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), vec4(0.0));
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    ShaderBase_PSMain();
    _streams.ColorTarget_id1 = ShadingBase_Shading();
    _streams.ColorTarget1_id2 = o1S2C1_ComputeColor_Compute();
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    PS_OUT_ColorTarget1 = _streams.ColorTarget1_id2;
}

