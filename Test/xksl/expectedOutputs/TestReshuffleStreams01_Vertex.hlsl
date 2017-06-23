struct VS_STREAMS
{
    float2 Position_id0;
    float2 VertexPosition_id1;
    float aFloat_id2;
};

static float2 VS_IN_VertexPosition;
static float VS_IN_aFloat;
static float2 VS_OUT_Position;

struct SPIRV_Cross_Input
{
    float2 VS_IN_VertexPosition : V_POSITION;
    float VS_IN_aFloat : V_AFLOAT;
};

struct SPIRV_Cross_Output
{
    float2 VS_OUT_Position : POSITION;
};

void vert_main()
{
    VS_STREAMS _streams = { float2(0.0f, 0.0f), float2(0.0f, 0.0f), 0.0f };
    _streams.VertexPosition_id1 = VS_IN_VertexPosition;
    _streams.aFloat_id2 = VS_IN_aFloat;
    _streams.Position_id0 = float2(0.0f, 1.0f + _streams.aFloat_id2) + _streams.VertexPosition_id1;
    VS_OUT_Position = _streams.Position_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_VertexPosition = stage_input.VS_IN_VertexPosition;
    VS_IN_aFloat = stage_input.VS_IN_aFloat;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_Position = VS_OUT_Position;
    return stage_output;
}
