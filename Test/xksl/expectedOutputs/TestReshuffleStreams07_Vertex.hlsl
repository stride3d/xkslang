struct VS_STREAMS
{
    float2x3 aMat23_id0;
};

static float2x3 VS_IN_aMat23;
static float2x3 VS_OUT_aMat23;

struct SPIRV_Cross_Input
{
    float2x3 VS_IN_aMat23 : AMAT23;
};

struct SPIRV_Cross_Output
{
    float2x3 VS_OUT_aMat23 : AMAT23;
};

void vert_main()
{
    VS_STREAMS _streams = { float2x3(0.0f.xxx, 0.0f.xxx) };
    _streams.aMat23_id0 = VS_IN_aMat23;
    VS_OUT_aMat23 = _streams.aMat23_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_aMat23[0] = stage_input.VS_IN_aMat23_0;
    VS_IN_aMat23[1] = stage_input.VS_IN_aMat23_1;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aMat23 = VS_OUT_aMat23;
    return stage_output;
}
