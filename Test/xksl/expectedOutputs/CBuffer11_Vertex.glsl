#version 450

struct VS_STREAMS
{
    float aStream_id0;
};

layout(std140) uniform PerLighting
{
    float ShaderMain_var1;
    float ShaderMain_var2;
    float ShaderMain_var3;
    float ShaderMain_var4;
} PerLighting_var;

layout(location = 0) out float VS_OUT_aStream;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0);
    _streams.aStream_id0 = PerLighting_var.ShaderMain_var1 + PerLighting_var.ShaderMain_var3;
    VS_OUT_aStream = _streams.aStream_id0;
}

