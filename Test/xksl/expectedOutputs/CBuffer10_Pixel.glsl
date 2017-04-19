#version 450

struct VS_OUT
{
    float aStream2_id0;
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

void main(VS_OUT __input__)
{
    PS_STREAMS _streams;
    _streams.aStream2_id0 = __input__.aStream2_id0;
    _streams.aStream3_id1 = _streams.aStream2_id0 + globalCbuffer_var.ShaderMain_var5;
}

