#version 450

struct VS_STREAMS
{
    vec4 LocalColor_id0;
    vec4 LocalColorBase_id1;
};

layout(location = 0) in vec4 VS_IN_LocalColor;
layout(location = 1) in vec4 VS_IN_LocalColorBase;

vec4 ShaderBase_abgr__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColorBase_id1.wzyx;
}

vec4 ShaderMain_wxyz_abgr__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wxyz + ShaderBase_abgr__Compute(_streams);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.LocalColor_id0 = VS_IN_LocalColor;
    _streams.LocalColorBase_id1 = VS_IN_LocalColorBase;
    vec4 color = ShaderMain_wxyz_abgr__Compute(_streams);
}

