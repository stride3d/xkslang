struct VS_STREAMS
{
    int aStream_id0;
};

static const VS_STREAMS _20 = { 0 };

cbuffer PerView
{
    int ShaderBase_var1;
    int ShaderBase_var2;
    float4 ShaderMain_padding_PerView_Default;
    int ShaderMain_var3;
    int ShaderMain_var4;
    float4 ShaderMain_padding_PerView_Lighting;
};

static int VS_OUT_aStream;

struct SPIRV_Cross_Output
{
    int VS_OUT_aStream : ASTREAM;
};

void vert_main()
{
    VS_STREAMS _streams = _20;
    _streams.aStream_id0 = ShaderBase_var1 + ShaderMain_var3;
    VS_OUT_aStream = _streams.aStream_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream = VS_OUT_aStream;
    return stage_output;
}
