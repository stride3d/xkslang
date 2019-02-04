struct PS_STREAMS
{
    float4 ShadingPosition_id0;
    float4 ColorTarget_id1;
    float Depth_id2;
};

static const PS_STREAMS _44 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f };

cbuffer Globals
{
    float ShaderMain_depthOffset;
};

static float gl_FragDepth;
static float4 PS_IN_SV_Position;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_SV_Position : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
    float gl_FragDepth : SV_Depth;
};

void frag_main()
{
    PS_STREAMS _streams = _44;
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.Depth_id2 = _streams.ShadingPosition_id0.w + ShaderMain_depthOffset;
    _streams.ColorTarget_id1 = float4(_streams.ShadingPosition_id0.x, _streams.ShadingPosition_id0.y, 1.0f, ShaderMain_depthOffset);
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
    gl_FragDepth = _streams.Depth_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_SV_Position = stage_input.PS_IN_SV_Position;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_FragDepth = gl_FragDepth;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
