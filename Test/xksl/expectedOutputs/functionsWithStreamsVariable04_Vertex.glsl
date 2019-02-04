#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct ShaderMain_Streams
{
    vec4 s1;
    vec4 b1;
    int _unused;
};

struct VS_STREAMS
{
    vec4 s1_id0;
    vec4 b1_id1;
};

in vec4 VS_IN_S1;
in vec4 VS_IN_B1;

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.s1_id0, _streams.b1_id1, 0);
    return res;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.s1_id0 = VS_IN_S1;
    _streams.b1_id1 = VS_IN_B1;
    ShaderMain_Streams s1 = ShaderMain__getStreams(_streams);
    ShaderMain_Streams s2 = ShaderMain__getStreams(_streams);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

