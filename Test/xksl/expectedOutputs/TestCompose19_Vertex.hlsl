struct VS_STREAMS
{
    float totoA_id0;
};

cbuffer Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
    float o2S55C0_ShaderComp_varC;
    float o3S55C1_ShaderComp_varC;
};

static float VS_OUT_totoA;

struct SPIRV_Cross_Output
{
    float VS_OUT_totoA : TOTOA;
};

float ShaderA_Compute()
{
    return 5.0f;
}

float o0S5C0_ShaderComp_Compute()
{
    return o0S5C0_ShaderComp_varC;
}

float o1S5C1_ShaderComp_Compute()
{
    return o1S5C1_ShaderComp_varC;
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f };
    float f = ShaderA_Compute();
    _streams.totoA_id0 = (f + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
    VS_OUT_totoA = _streams.totoA_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_totoA = VS_OUT_totoA;
    return stage_output;
}
