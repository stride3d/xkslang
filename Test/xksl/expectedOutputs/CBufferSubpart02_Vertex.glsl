#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    int aStream_id0;
};

layout(std140) uniform PerView
{
    int ShaderBase_var1;
    int ShaderBase_var2;
    vec4 ShaderMain_padding_PerView_Default;
    int ShaderMain_var3;
    int ShaderMain_var4;
    vec4 ShaderMain_padding_PerView_Lighting;
} PerView_var;

out int VS_OUT_aStream;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0);
    _streams.aStream_id0 = PerView_var.ShaderBase_var1 + PerView_var.ShaderMain_var3;
    VS_OUT_aStream = _streams.aStream_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

