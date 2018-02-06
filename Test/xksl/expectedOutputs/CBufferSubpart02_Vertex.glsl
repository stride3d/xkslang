#version 450

struct VS_STREAMS
{
    int aStream_id0;
};

layout(std140) uniform PerView
{
    int ShaderBase_var1;
    int ShaderBase_var2;
    vec4 ShaderMain__padding_PerView_Default;
    int ShaderMain_var3;
    int ShaderMain_var4;
    vec4 ShaderMain__padding_PerView_Lighting;
} PerView_var;

layout(location = 0) out int VS_OUT_aStream;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0);
    _streams.aStream_id0 = PerView_var.ShaderBase_var1 + PerView_var.ShaderMain_var3;
    VS_OUT_aStream = _streams.aStream_id0;
}

