struct PS_STREAMS
{
    float4 ColorTarget_id0;
};

cbuffer PerDraw
{
    float2x3 ShaderMain_aMat23;
    float2x3 ShaderMain_aMat23_rm;
    float4 ShaderMain_Color;
};

static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.ColorTarget_id0 = ShaderMain_Color;
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
