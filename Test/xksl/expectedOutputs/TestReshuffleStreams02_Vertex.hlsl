struct VS_STREAMS
{
    float2 Position_id0;
};

static float2 VS_OUT_Position;

struct SPIRV_Cross_Output
{
    float2 VS_OUT_Position : POSITION;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xx };
    _streams.Position_id0 = float2(0.0f, 1.0f);
    VS_OUT_Position = _streams.Position_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_Position = VS_OUT_Position;
    return stage_output;
}
