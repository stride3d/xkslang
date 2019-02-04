#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 LocalColor_id0;
    vec4 LocalColorBase_id1;
};

in vec4 VS_IN_TStream;
in vec4 VS_IN_TStreamB;

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
    _streams.LocalColor_id0 = VS_IN_TStream;
    _streams.LocalColorBase_id1 = VS_IN_TStreamB;
    vec4 color = ShaderMain_wxyz_abgr__Compute(_streams);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

