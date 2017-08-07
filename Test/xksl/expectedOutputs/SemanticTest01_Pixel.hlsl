struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float Depth_id2;
    float CustomStream_id3;
};

static float4 PS_IN_ShadingPosition;
static float4 PS_OUT_ColorTarget;
static float PS_OUT_Depth;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
    float PS_OUT_Depth : SV_Depth;
};

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    _streams.CustomStream_id3 = 1.0f;
    _streams.Depth_id2 = 0.0f;
    _streams.ColorTarget_id1 = float4(_streams.ShadingPosition_id0.x, _streams.ShadingPosition_id0.y, 1.0f, 1.0f);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    PS_OUT_Depth = _streams.Depth_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    stage_output.PS_OUT_Depth = PS_OUT_Depth;
    return stage_output;
}
