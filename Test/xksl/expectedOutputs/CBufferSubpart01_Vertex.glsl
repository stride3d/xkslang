#version 450

struct VS_STREAMS
{
    int aStream_id0;
};

layout(std140) uniform PerLighting
{
    int ShaderMain_var7;
    int ShaderMain_var8;
    vec4 ShaderMain__padding_PerLighting_Default;
    int ShaderMain_var3;
    int ShaderMain_var4;
    vec4 ShaderMain__padding_PerLighting_subpart2;
    int ShaderMain_var1;
    int ShaderMain_var2;
    vec4 ShaderMain__padding_PerLighting_subpart1;
    int ShaderMain_var9;
    int ShaderMain_var10;
    int ShaderMain_var5;
    int ShaderMain_var6;
    vec4 ShaderMain__padding_PerLighting_subpart3;
} PerLighting_var;

layout(location = 0) out int VS_OUT_aStream;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0);
    _streams.aStream_id0 = (((PerLighting_var.ShaderMain_var1 + PerLighting_var.ShaderMain_var3) + PerLighting_var.ShaderMain_var5) + PerLighting_var.ShaderMain_var7) + PerLighting_var.ShaderMain_var9;
    VS_OUT_aStream = _streams.aStream_id0;
}

