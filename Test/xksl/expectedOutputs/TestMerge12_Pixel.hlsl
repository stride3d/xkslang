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

int A1_Compute()
{
    return Base_Compute() + 2;
}

int B1_Compute()
{
    return A1_Compute() + 3;
}

int A2_Compute()
{
    return B1_Compute() + 20;
}

int B2_Compute()
{
    return A2_Compute() + 30;
}

int C_Compute()
{
    return B2_Compute() + 100;
}

void frag_main()
{
    PS_STREAMS _streams = { 0 };
    _streams.ColorTarget_id0 = C_Compute();
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
