#version 450

struct VS_STREAMS
{
    vec4 LocalColor_id0;
};

layout(location = 0) in vec4 VS_IN_LocalColor;

vec4 ShaderBase_abgr_UPDATED_SEMANTIC__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wzyx;
}

vec4 ShaderMain_wxyz_abgr_UPDATED_SEMANTIC__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wxyz + ShaderBase_abgr_UPDATED_SEMANTIC__Compute(_streams);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.LocalColor_id0 = VS_IN_LocalColor;
    vec4 color = ShaderMain_wxyz_abgr_UPDATED_SEMANTIC__Compute(_streams);
}

