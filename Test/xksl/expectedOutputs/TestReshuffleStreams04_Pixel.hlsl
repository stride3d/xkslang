struct PS_STREAMS
{
    float2 Position_id0;
    float4 ColorTarget_id1;
};

static float2 PS_IN_Position;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float2 PS_IN_Position : POSITION;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float4 ShaderMain_Compute(PS_STREAMS _streams, float p)
{
    return float4(_streams.Position_id0, p, 1.0f);
}

float4 ShaderMain_Compute(PS_STREAMS _streams)
{
    float param = 5.0f;
    return ShaderMain_Compute(_streams, param);
}

void frag_main()
{
    PS_STREAMS _streams = { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.Position_id0 = PS_IN_Position;
    _streams.ColorTarget_id1 = ShaderMain_Compute(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_Position = stage_input.PS_IN_Position;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
