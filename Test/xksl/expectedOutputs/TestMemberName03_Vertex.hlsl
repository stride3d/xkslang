struct VS_STREAMS
{
    float4 LocalColor_id0;
};

static float4 VS_IN_LocalColor;

struct SPIRV_Cross_Input
{
    float4 VS_IN_LocalColor : TStream;
};

float4 ShaderMain_wxyz__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wxyz;
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx };
    _streams.LocalColor_id0 = VS_IN_LocalColor;
    float4 color = ShaderMain_wxyz__Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_LocalColor = stage_input.VS_IN_LocalColor;
    vert_main();
}
