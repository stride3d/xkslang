struct VS_STREAMS
{
    float2 Position_id0;
    float2 VertexPosition_id1;
    float aFloat_id2;
};

static const VS_STREAMS _33 = { 0.0f.xx, 0.0f.xx, 0.0f };

static float2 VS_IN_V_POSITION;
static float VS_IN_V_AFLOAT;
static float2 VS_OUT_Position;

struct SPIRV_Cross_Input
{
    float VS_IN_V_AFLOAT : V_AFLOAT;
    float2 VS_IN_V_POSITION : V_POSITION;
};

struct SPIRV_Cross_Output
{
    float2 VS_OUT_Position : POSITION;
};

void vert_main()
{
    VS_STREAMS _streams = _33;
    _streams.VertexPosition_id1 = VS_IN_V_POSITION;
    _streams.aFloat_id2 = VS_IN_V_AFLOAT;
    _streams.Position_id0 = float2(0.0f, 1.0f + _streams.aFloat_id2) + _streams.VertexPosition_id1;
    VS_OUT_Position = _streams.Position_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_V_POSITION = stage_input.VS_IN_V_POSITION;
    VS_IN_V_AFLOAT = stage_input.VS_IN_V_AFLOAT;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_Position = VS_OUT_Position;
    return stage_output;
}
