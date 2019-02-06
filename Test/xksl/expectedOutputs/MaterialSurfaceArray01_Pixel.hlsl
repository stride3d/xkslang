struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
};

static const PS_STREAMS _31 = { 0.0f.xxxx, 0.0f.xxxx };

static float4 PS_IN_SV_Position;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_SV_Position : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void ShaderBase_PSMain()
{
}

float4 o0S27C0_ComputeColor_Compute()
{
    return 0.0f.xxxx;
}

float4 ShadingBase_Shading()
{
    return o0S27C0_ComputeColor_Compute();
}

void frag_main()
{
    PS_STREAMS _streams = _31;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    ShaderBase_PSMain();
    _streams.ColorTarget_id1 = ShadingBase_Shading();
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
