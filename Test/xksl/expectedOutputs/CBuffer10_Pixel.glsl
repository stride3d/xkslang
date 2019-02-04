#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    float aStream2_id0;
    float aStream3_id1;
};

layout(std140) uniform Globals
{
    float ShaderMain_var5;
} Globals_var;

in float PS_IN_ASTREAM2;
out float PS_OUT_aStream3;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0.0);
    _streams.aStream2_id0 = PS_IN_ASTREAM2;
    _streams.aStream3_id1 = _streams.aStream2_id0 + Globals_var.ShaderMain_var5;
    PS_OUT_aStream3 = _streams.aStream3_id1;
}

