struct VS_STREAMS
{
    float2 TexCoord_id0;
    float2 aStream_id1;
};

static float2 VS_IN_TexCoord;
static float2 VS_OUT_aStream;

struct SPIRV_Cross_Input
{
    float2 VS_IN_TexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float2 VS_OUT_aStream : ASTREAM;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xx, 0.0f.xx };
    _streams.TexCoord_id0 = VS_IN_TexCoord;
    _streams.aStream_id1 = _streams.TexCoord_id0;
    VS_OUT_aStream = _streams.aStream_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TexCoord = stage_input.VS_IN_TexCoord;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream = VS_OUT_aStream;
    return stage_output;
}
