#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    int sa1_id0;
};

layout(std140) uniform Globals
{
    int shaderA_Var0;
} Globals_var;

in int PS_IN_SA1;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.sa1_id0 = PS_IN_SA1;
    int a = ((Globals_var.shaderA_Var0 + _streams.sa1_id0) + 2) + 3;
}

