struct VS_STREAMS
{
    float4 LocalColor_id0;
};

static float4 VS_IN_LocalColor;

struct SPIRV_Cross_Input
{
    float4 VS_IN_LocalColor : CUSTOM_SEMANTIC;
};

float4 ShaderMain_CUSTOM_SEMANTIC__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0;
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.LocalColor_id0 = VS_IN_LocalColor;
    float4 color = ShaderMain_CUSTOM_SEMANTIC__Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_LocalColor = stage_input.VS_IN_LocalColor;
    vert_main();
}
