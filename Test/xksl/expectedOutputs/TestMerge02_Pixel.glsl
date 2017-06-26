#version 450

struct PS_STREAMS
{
    int sa1_id0;
};

layout(std140) uniform Globals
{
    int shaderA_Var0;
} Globals_var;

layout(location = 0) in int PS_IN_sa1;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.sa1_id0 = PS_IN_sa1;
    int a = ((Globals_var.shaderA_Var0 + _streams.sa1_id0) + 2) + 3;
}

