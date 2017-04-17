#version 450

struct globalStreams
{
    int aStream_0;
    int aStream_1;
};

layout(std140) uniform PreDraw
{
    int ShaderBase_1__aVar;
} PreDraw_var;

globalStreams globalStreams_var;

int ShaderBase_1__compute()
{
    return (PreDraw_var.ShaderBase_1__aVar + 1) + globalStreams_var.aStream_0;
}

int ShaderMain_1_2__compute()
{
    return ShaderBase_1__compute();
}

int main()
{
    return ShaderMain_1_2__compute();
}

