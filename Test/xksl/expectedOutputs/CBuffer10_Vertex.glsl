#version 450

struct VS_IN
{
    float aStream1_id0;
};

struct VS_OUT
{
    float aStream2_id0;
};

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

VS_OUT main(VS_IN __input__)
{
    VS_STREAMS _streams = VS_STREAMS(0.0, 0.0);
    _streams.aStream1_id0 = __input__.aStream1_id0;
    _streams.aStream2_id1 = _streams.aStream1_id0 + PerLighting_var.ShaderMain_var1;
    VS_OUT __output__ = VS_OUT(0.0);
    __output__.aStream2_id0 = _streams.aStream2_id1;
    return __output__;
}

