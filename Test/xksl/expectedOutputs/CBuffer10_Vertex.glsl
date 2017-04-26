#version 450

struct VS_STREAMS
{
    float aStream1_id0;
    float aStream2_id1;
};

layout(std140) uniform PerLighting
{
    float ShaderMain_var1;
    float ShaderMain_var2;
} PerLighting_var;

layout(location = 0) in float VS_IN_aStream1;
layout(location = 0) out float VS_OUT_aStream2;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, 0.0);
    _streams.aStream1_id0 = VS_IN_aStream1;
    _streams.aStream2_id1 = _streams.aStream1_id0 + PerLighting_var.ShaderMain_var1;
    VS_OUT_aStream2 = _streams.aStream2_id1;
}

