struct VS_STREAMS
{
    float2 Position_id0;
};

static const VS_STREAMS _18 = { 0.0f.xx };

void vert_main()
{
    VS_STREAMS _streams = _18;
    _streams.Position_id0 = float2(0.0f, 1.0f);
}

void main()
{
    vert_main();
}
