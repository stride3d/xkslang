struct VS_STREAMS
{
    float aStream_id0;
};

cbuffer PerLighting
{
    float ShaderMain_var1;
    float ShaderMain_var2;
    float ShaderMain_var3;
    float ShaderMain_var4;
};

static float VS_OUT_aStream;

struct SPIRV_Cross_Output
{
    float VS_OUT_aStream : TEXCOORD0;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f };
    _streams.aStream_id0 = ShaderMain_var1 + ShaderMain_var3;
    VS_OUT_aStream = _streams.aStream_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream = VS_OUT_aStream;
    return stage_output;
}
