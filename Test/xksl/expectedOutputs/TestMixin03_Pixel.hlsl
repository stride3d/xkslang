struct PS_STREAMS
{
    int ColorTarget_id0;
};

static int PS_OUT_ColorTarget;

struct SPIRV_Cross_Output
{
    int PS_OUT_ColorTarget : SV_Target0;
};

int Base_Compute()
{
    return 1;
}

int ShaderA_Compute()
{
    return Base_Compute() + 3;
}

void frag_main()
{
    PS_STREAMS _streams = { 0 };
    _streams.ColorTarget_id0 = ShaderA_Compute();
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
