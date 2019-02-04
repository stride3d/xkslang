struct VS_STREAMS
{
    float4 LocalColor_id0;
    float4 LocalColorBase_id1;
};

static const VS_STREAMS _33 = { 0.0f.xxxx, 0.0f.xxxx };

static float4 VS_IN_TStream;
static float4 VS_IN_TStreamB;

struct SPIRV_Cross_Input
{
    float4 VS_IN_TStream : TStream;
    float4 VS_IN_TStreamB : TStreamB;
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
    VS_STREAMS _streams = _33;
    _streams.LocalColor_id0 = VS_IN_TStream;
    _streams.LocalColorBase_id1 = VS_IN_TStreamB;
    float4 color = ShaderMain_wxyz_abgr__Compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TStream = stage_input.VS_IN_TStream;
    VS_IN_TStreamB = stage_input.VS_IN_TStreamB;
    vert_main();
}
