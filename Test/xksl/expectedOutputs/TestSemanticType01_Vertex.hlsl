struct VS_STREAMS
{
    float4 LocalColor_id0;
};

static const VS_STREAMS _23 = { 0.0f.xxxx };

static float4 VS_IN_CUSTOM_SEMANTIC;

struct SPIRV_Cross_Input
{
    float4 VS_IN_CUSTOM_SEMANTIC : CUSTOM_SEMANTIC;
};

float4 ShaderMain_CUSTOM_SEMANTIC__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0;
}

void vert_main()
{
    VS_STREAMS _streams = _23;
    _streams.LocalColor_id0 = VS_IN_CUSTOM_SEMANTIC;
    float4 color = ShaderMain_CUSTOM_SEMANTIC__Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_CUSTOM_SEMANTIC = stage_input.VS_IN_CUSTOM_SEMANTIC;
    vert_main();
}
