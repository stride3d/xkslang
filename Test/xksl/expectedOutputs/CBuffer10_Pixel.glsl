#version 450

struct VS_OUT
{
    float aStream2_id0;
};

struct PS_OUT
{
    float aStream3_id0;
};

struct PS_STREAMS
{
    float aStream2_id0;
    float aStream3_id1;
};

layout(std140) uniform globalCbuffer
{
    float ShaderMain_var5;
} globalCbuffer_var;

PS_OUT main(VS_OUT __input__)
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0.0);
    _streams.aStream2_id0 = __input__.aStream2_id0;
    _streams.aStream3_id1 = _streams.aStream2_id0 + globalCbuffer_var.ShaderMain_var5;
    PS_OUT __output__ = PS_OUT(0.0);
    __output__.aStream3_id0 = _streams.aStream3_id1;
    return __output__;
}

