struct VS_STREAMS
{
    int aStream_id0;
};

cbuffer PerView
{
    int ShaderBase_var1;
    int ShaderBase_var2;
    float4 ShaderMain__padding_PerView_Default;
    int ShaderMain_var3;
    int ShaderMain_var4;
    float4 ShaderMain__padding_PerView_Lighting;
};

static int VS_OUT_aStream;

struct SPIRV_Cross_Output
{
    int VS_OUT_aStream : ASTREAM;
};

void vert_main()
{
    VS_STREAMS _streams = { 0 };
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
