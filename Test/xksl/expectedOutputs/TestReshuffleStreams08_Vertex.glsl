#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    float s1_id0;
    float s3_id1;
};

void ShaderMain_CommonFunctionUsingStreams(inout VS_STREAMS _streams, float f)
{
    _streams.s3_id1 = f;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, 0.0);
    _streams.s1_id0 = 0.0;
    float param = 1.0;
    ShaderMain_CommonFunctionUsingStreams(_streams, param);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

