struct PS_STREAMS
{
    float4 ColorTarget_id0;
};

cbuffer PerDraw
{
    column_major float2x3 ShaderMain_aMat23[2];
    float4 ShaderMain_Color[4];
    float4 ShaderMain_ColorArray[2];
};

static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx };
    _streams.ColorTarget_id0 = ShaderMain_Color[1];
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
