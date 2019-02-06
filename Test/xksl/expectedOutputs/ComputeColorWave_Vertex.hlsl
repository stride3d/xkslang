struct VS_STREAMS
{
    float2 TexCoord_id0;
};

static const VS_STREAMS _19 = { 0.0f.xx };

static float2 VS_IN_TEXCOORD0;
static float2 VS_OUT_TexCoord;

struct SPIRV_Cross_Input
{
    float2 VS_IN_TEXCOORD0 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float2 VS_OUT_TexCoord : TEXCOORD0;
};

void vert_main()
{
    VS_STREAMS _streams = _19;
    _streams.TexCoord_id0 = VS_IN_TEXCOORD0;
    VS_OUT_TexCoord = _streams.TexCoord_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TEXCOORD0 = stage_input.VS_IN_TEXCOORD0;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_TexCoord = VS_OUT_TexCoord;
    return stage_output;
}
