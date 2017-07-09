struct VS_STREAMS
{
    float4 LocalColor_id0;
    float4 LocalColorBase_id1;
};

static float4 VS_IN_LocalColor;
static float4 VS_IN_LocalColorBase;

struct SPIRV_Cross_Input
{
    float4 VS_IN_LocalColor : TStream;
    float4 VS_IN_LocalColorBase : TStreamB;
};

float4 ShaderBase_abgr__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColorBase_id1.wzyx;
}

float4 ShaderMain_wxyz_abgr__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wxyz + ShaderBase_abgr__Compute(_streams);
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.LocalColor_id0 = VS_IN_LocalColor;
    _streams.LocalColorBase_id1 = VS_IN_LocalColorBase;
    float4 color = ShaderMain_wxyz_abgr__Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_LocalColor = stage_input.VS_IN_LocalColor;
    VS_IN_LocalColorBase = stage_input.VS_IN_LocalColorBase;
    vert_main();
}
