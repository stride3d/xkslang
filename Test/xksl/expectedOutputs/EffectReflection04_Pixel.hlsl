struct ShaderMain_StructType
{
    float aFloat;
    int aInt;
    uint aBool;
};

struct PS_STREAMS
{
    float4 ColorTarget_id0;
};

cbuffer Globals
{
    ShaderMain_StructType ShaderMain_var1;
    ShaderMain_StructType ShaderMain_var2;
};

static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx };
    _streams.ColorTarget_id0 = float4(ShaderMain_var1.aFloat, ShaderMain_var2.aFloat, 0.0f, 0.0f);
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

SPIRV_Cross_Output main()
{
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
