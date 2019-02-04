struct VS_STREAMS
{
    float2 Position_id0;
};

static const VS_STREAMS _20 = { 0.0f.xx };

static float4 gl_Position;
struct SPIRV_Cross_Output
{
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    VS_STREAMS _streams = _20;
    _streams.Position_id0 = float2(0.0f, 1.0f);
    gl_Position = _streams.Position_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    return stage_output;
}
