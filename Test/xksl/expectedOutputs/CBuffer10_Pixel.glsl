#version 450

struct PS_STREAMS
{
    float aStream2_id0;
    float aStream3_id1;
};

layout(std140) uniform globalCbuffer
{
    float ShaderMain_var5;
} globalCbuffer_var;

layout(location = 0) in float PS_IN_aStream2;
layout(location = 0) out float PS_OUT_aStream3;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0.0);
    _streams.aStream2_id0 = PS_IN_aStream2;
    _streams.aStream3_id1 = _streams.aStream2_id0 + globalCbuffer_var.ShaderMain_var5;
    PS_OUT_aStream3 = _streams.aStream3_id1;
}

