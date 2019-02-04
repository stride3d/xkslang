#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    int aStream_id0;
};

layout(std140) uniform PreDraw
{
    int ShaderBase_aVar;
} PreDraw_var;

in int VS_IN_ASTREAM;

int ShaderBase_1__compute(VS_STREAMS _streams)
{
    return (PreDraw_var.ShaderBase_aVar + 1) + _streams.aStream_id0;
}

int ShaderMain_1_1__compute(VS_STREAMS _streams)
{
    return ShaderBase_1__compute(_streams);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0);
    _streams.aStream_id0 = VS_IN_ASTREAM;
    int i = ShaderMain_1_1__compute(_streams);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

