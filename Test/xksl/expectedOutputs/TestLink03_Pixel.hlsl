struct PS_STREAMS
{
    float4 s_int_id0;
    float4 s_out_id1;
};

static float4 PS_IN_s_int;
static float4 PS_OUT_s_out;

struct SPIRV_Cross_Input
{
    float4 PS_IN_s_int : S_INT;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_s_out : S_OUTPUT;
};

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.s_int_id0 = PS_IN_s_int;
    _streams.s_out_id1 = _streams.s_int_id0;
    PS_OUT_s_out = _streams.s_out_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_s_int = stage_input.PS_IN_s_int;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_s_out = PS_OUT_s_out;
    return stage_output;
}
