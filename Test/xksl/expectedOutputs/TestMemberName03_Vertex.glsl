#version 450

struct VS_STREAMS
{
    vec4 LocalColor_id0;
};

layout(location = 0) in vec4 VS_IN_LocalColor;

vec4 ShaderMain_wxyz__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wxyz;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.LocalColor_id0 = VS_IN_LocalColor;
    vec4 color = ShaderMain_wxyz__Compute(_streams);
}

