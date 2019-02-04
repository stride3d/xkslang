struct VS_STREAMS
{
    float4 LocalColor_id0;
};

static const VS_STREAMS _24 = { 0.0f.xxxx };

static float4 VS_IN_TStream;

struct SPIRV_Cross_Input
{
    float4 VS_IN_TStream : TStream;
};

float4 ShaderMain_wxyz__Compute(VS_STREAMS _streams)
{
    return _streams.LocalColor_id0.wxyz;
}

void vert_main()
{
    VS_STREAMS _streams = _24;
    _streams.LocalColor_id0 = VS_IN_TStream;
    float4 color = ShaderMain_wxyz__Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TStream = stage_input.VS_IN_TStream;
    vert_main();
}
