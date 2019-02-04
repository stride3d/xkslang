struct PS_STREAMS
{
    float aStream2_id0;
    float aStream3_id1;
};

static const PS_STREAMS _25 = { 0.0f, 0.0f };

cbuffer Globals
{
    float ShaderMain_var5;
};

static float PS_IN_ASTREAM2;
static float PS_OUT_aStream3;

struct SPIRV_Cross_Input
{
    float PS_IN_ASTREAM2 : ASTREAM2;
};

struct SPIRV_Cross_Output
{
    float PS_OUT_aStream3 : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = _25;
    _streams.aStream2_id0 = PS_IN_ASTREAM2;
    _streams.aStream3_id1 = _streams.aStream2_id0 + ShaderMain_var5;
    PS_OUT_aStream3 = _streams.aStream3_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ASTREAM2 = stage_input.PS_IN_ASTREAM2;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_aStream3 = PS_OUT_aStream3;
    return stage_output;
}
