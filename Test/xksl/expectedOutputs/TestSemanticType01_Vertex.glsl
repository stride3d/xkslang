#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 LocalColor_id0;
};

in vec4 VS_IN_CUSTOM_SEMANTIC;

vec4 ShaderMain_CUSTOM_SEMANTIC__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.LocalColor_id0 = VS_IN_CUSTOM_SEMANTIC;
    vec4 color = ShaderMain_CUSTOM_SEMANTIC__Compute(_streams);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

