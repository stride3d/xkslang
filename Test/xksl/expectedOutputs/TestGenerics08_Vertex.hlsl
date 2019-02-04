struct VS_STREAMS
{
    int aStream_id0;
};

static const VS_STREAMS _28 = { 0 };

cbuffer PreDraw
{
    int ShaderBase_aVar;
};

static int VS_IN_ASTREAM;

struct SPIRV_Cross_Input
{
    int VS_IN_ASTREAM : ASTREAM;
};

int ShaderBase_1__compute(VS_STREAMS _streams)
{
    return (ShaderBase_aVar + 1) + _streams.aStream_id0;
}

int ShaderMain_1_1__compute(VS_STREAMS _streams)
{
    return ShaderBase_1__compute(_streams);
}

void vert_main()
{
    VS_STREAMS _streams = _28;
    _streams.aStream_id0 = VS_IN_ASTREAM;
    int i = ShaderMain_1_1__compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_ASTREAM = stage_input.VS_IN_ASTREAM;
    vert_main();
}
