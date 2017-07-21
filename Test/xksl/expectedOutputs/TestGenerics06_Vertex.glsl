#version 450

struct VS_STREAMS
{
    int aStream_id0;
};

layout(std140) uniform PreDraw
{
    int ShaderBase_aVar;
} PreDraw_var;

layout(location = 0) out int VS_OUT_aStream;

int ShaderBase_4_1__compute()
{
    return PreDraw_var.ShaderBase_aVar + 4;
}

int ShaderMain_7_4__compute()
{
    return ShaderBase_4_1__compute();
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0);
    _streams.aStream_id0 = ShaderMain_7_4__compute() + 11;
    VS_OUT_aStream = _streams.aStream_id0;
}

