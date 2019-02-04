struct PS_STREAMS
{
    float totoA_id0;
    float totoB_id1;
};

static const PS_STREAMS _42 = { 0.0f, 0.0f };

cbuffer Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
    float o2S55C0_ShaderComp_varC;
    float o3S55C1_ShaderComp_varC;
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

float ShaderB_Compute()
{
    return 5.0f;
}

float o2S55C0_ShaderComp_Compute()
{
    return o2S55C0_ShaderComp_varC;
}

float o3S55C1_ShaderComp_Compute()
{
    return o3S55C1_ShaderComp_varC;
}

void frag_main()
{
    PS_STREAMS _streams = _42;
    _streams.totoA_id0 = PS_IN_TOTOA;
    float f = ShaderB_Compute();
    _streams.totoB_id1 = ((f + o2S55C0_ShaderComp_Compute()) + o3S55C1_ShaderComp_Compute()) + _streams.totoA_id0;
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
