struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float4 ColorTarget1_id2;
};

static float4 PS_IN_ShadingPosition;
static float4 PS_OUT_ColorTarget;
static float4 PS_OUT_ColorTarget1;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
    float4 PS_OUT_ColorTarget1 : SV_Target1;
};

void ShaderBase_PSMain()
{
}

float4 o0S2C0_ComputeColor_Compute()
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float4 ShadingBase_Shading()
{
    return o0S2C0_ComputeColor_Compute();
}

float4 o1S2C1_ComputeColor_Compute()
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.ShadingPosition_id0 = PS_IN_ShadingPosition;
    ShaderBase_PSMain();
    _streams.ColorTarget_id1 = ShadingBase_Shading();
    _streams.ColorTarget1_id2 = o1S2C1_ComputeColor_Compute();
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    PS_OUT_ColorTarget1 = _streams.ColorTarget1_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    stage_output.PS_OUT_ColorTarget1 = PS_OUT_ColorTarget1;
    return stage_output;
}
