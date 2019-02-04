struct VS_STREAMS
{
    float4 LocalColor_id0;
};

static const VS_STREAMS _31 = { 0.0f.xxxx };

static float4 VS_IN_UPDATED_SEMANTIC;

struct SPIRV_Cross_Input
{
    float4 VS_IN_UPDATED_SEMANTIC : UPDATED_SEMANTIC;
};

float4 ShaderBase_abgr_UPDATED_SEMANTIC__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wzyx;
}

float4 ShaderMain_wxyz_abgr_UPDATED_SEMANTIC__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wxyz + ShaderBase_abgr_UPDATED_SEMANTIC__Compute(_streams);
}

void vert_main()
{
    VS_STREAMS _streams = _31;
    _streams.LocalColor_id0 = VS_IN_UPDATED_SEMANTIC;
    float4 color = ShaderMain_wxyz_abgr_UPDATED_SEMANTIC__Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_UPDATED_SEMANTIC = stage_input.VS_IN_UPDATED_SEMANTIC;
    vert_main();
}
