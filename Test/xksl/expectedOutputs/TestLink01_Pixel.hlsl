struct PS_STREAMS
{
    float4 s_int_id0;
    float4 s_out_id1;
};

static const PS_STREAMS _23 = { 0.0f.xxxx, 0.0f.xxxx };

static float4 PS_IN_S_INT;
static float4 PS_OUT_s_out;

struct SPIRV_Cross_Input
{
    float4 PS_IN_S_INT : S_INT;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_s_out : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = _23;
    _streams.s_int_id0 = PS_IN_S_INT;
    _streams.s_out_id1 = _streams.s_int_id0;
    PS_OUT_s_out = _streams.s_out_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_S_INT = stage_input.PS_IN_S_INT;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_s_out = PS_OUT_s_out;
    return stage_output;
}
