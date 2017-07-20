struct VS_STREAMS
{
    int aStream_id0;
};

cbuffer PreDraw
{
    int ShaderBase_id0_aVar;
};

static int VS_IN_aStream;

struct SPIRV_Cross_Input
{
    int VS_IN_aStream : TEXCOORD0;
};

int ShaderBase_1__compute(VS_STREAMS _streams)
{
    return (ShaderBase_id0_aVar + 1) + _streams.aStream_id0;
}

int ShaderMain_1_1__compute(VS_STREAMS _streams)
{
    return ShaderBase_1__compute(_streams);
}

void vert_main()
{
    VS_STREAMS _streams = { 0 };
    _streams.aStream_id0 = VS_IN_aStream;
    int i = ShaderMain_1_1__compute(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_aStream = stage_input.VS_IN_aStream;
    vert_main();
}
