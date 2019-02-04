#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct ShaderMain_Streams
{
    float matBlend;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
};

in float VS_IN_MATBLEND;
out float VS_OUT_matBlend;

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.matBlend_id0, 0);
    return res;
}

float ShaderMain_Compute(ShaderMain_Streams fromStream)
{
    return fromStream.matBlend;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0);
    _streams.matBlend_id0 = VS_IN_MATBLEND;
    ShaderMain_Streams backup = ShaderMain__getStreams(_streams);
    ShaderMain_Streams param = backup;
    float f = ShaderMain_Compute(param);
    ShaderMain_Streams param_1 = ShaderMain__getStreams(_streams);
    float f2 = ShaderMain_Compute(param_1);
    VS_OUT_matBlend = _streams.matBlend_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

