struct PS_STREAMS
{
    float totoA_id0;
    float totoB_id1;
};

static const PS_STREAMS _56 = { 0.0f, 0.0f };

cbuffer Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
    float o2S39C0_ShaderComp_varC;
    float o4S39C1_ShaderComp_varC;
    float o4S39C1_o3S15C0_ShaderComp_varC;
};

static float PS_IN_TOTOA;
static float PS_OUT_totoB;

struct SPIRV_Cross_Input
{
    float PS_IN_TOTOA : TOTOA;
};

struct SPIRV_Cross_Output
{
    float PS_OUT_totoB : SV_Target0;
};

float ShaderMain_Compute()
{
    return 1.0f;
}

float ShaderB_Compute()
{
    return ShaderMain_Compute() + 2.0f;
}

float o2S39C0_ShaderComp_Compute()
{
    return o2S39C0_ShaderComp_varC;
}

float o4S39C1_ShaderComp_Compute()
{
    return o4S39C1_ShaderComp_varC;
}

float o4S39C1_o3S15C0_ShaderComp_Compute()
{
    return o4S39C1_o3S15C0_ShaderComp_varC;
}

float o4S39C1_ShaderCompBis_Compute()
{
    return o4S39C1_ShaderComp_Compute() + o4S39C1_o3S15C0_ShaderComp_Compute();
}

void frag_main()
{
    PS_STREAMS _streams = _56;
    _streams.totoA_id0 = PS_IN_TOTOA;
    float f = ShaderB_Compute();
    _streams.totoB_id1 = ((f + o2S39C0_ShaderComp_Compute()) + o4S39C1_ShaderCompBis_Compute()) + _streams.totoA_id0;
    PS_OUT_totoB = _streams.totoB_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TOTOA = stage_input.PS_IN_TOTOA;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_totoB = PS_OUT_totoB;
    return stage_output;
}
