#version 450

struct globalStreams
{
    int aStream_0;
};

layout(std140) uniform PreDraw
{
    int ShaderBase_4_1__aVar;
} PreDraw_var;

globalStreams globalStreams_var;

int ShaderBase_4_1__compute()
{
    return PreDraw_var.ShaderBase_4_1__aVar + 4;
}

int ShaderMain_7_4__compute()
{
    return ShaderBase_4_1__compute();
}

void main()
{
    globalStreams_var.aStream_0 = ShaderMain_7_4__compute() + 11;
}

