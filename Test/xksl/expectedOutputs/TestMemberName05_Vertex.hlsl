struct VS_STREAMS
{
    float4 aStream1_id0;
    float4 outputStream_id1;
};

cbuffer Globals
{
    float4 ShaderMain_aCol;
};

static float4 VS_OUT_aStream1;
static float4 VS_OUT_outputStream;

struct SPIRV_Cross_Output
{
    float4 VS_OUT_aStream1 : ASTREAM1;
    float4 VS_OUT_outputStream : OUTPUTSTREAM;
};

void ShaderMain_aStream1__Compute(inout VS_STREAMS _streams)
{
    _streams.aStream1_id0 = ShaderMain_aCol;
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxxx };
    ShaderMain_aStream1__Compute(_streams);
    _streams.outputStream_id1 = _streams.aStream1_id0;
    VS_OUT_aStream1 = _streams.aStream1_id0;
    VS_OUT_outputStream = _streams.outputStream_id1;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream1 = VS_OUT_aStream1;
    stage_output.VS_OUT_outputStream = VS_OUT_outputStream;
    return stage_output;
}
