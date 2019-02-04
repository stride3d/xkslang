struct VS_STREAMS
{
    float4 outStreamA_id0;
};

static const VS_STREAMS _18 = { 0.0f.xxxx };

void vert_main()
{
    VS_STREAMS _streams = _18;
    _streams.outStreamA_id0 = 5.0f.xxxx;
}

void main()
{
    vert_main();
}
