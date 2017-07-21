#version 450

struct VS_STREAMS
{
    int aStream_id0;
};

layout(std140) uniform PreDraw
{
    int ShaderBase_aVar;
} PreDraw_var;

layout(location = 0) in int VS_IN_aStream;

int ShaderBase_1__compute(VS_STREAMS _streams)
{
    return (PreDraw_var.ShaderBase_aVar + 1) + _streams.aStream_id0;
}

int ShaderMain_1_2__compute(VS_STREAMS _streams)
{
    return ShaderBase_1__compute(_streams);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0);
    _streams.aStream_id0 = VS_IN_aStream;
    int i = ShaderMain_1_2__compute(_streams);
}

