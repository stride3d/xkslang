struct VS_STREAMS
{
    float2 Position_id0;
};

void vert_main()
{
    VS_STREAMS _streams = { float2(0.0f, 0.0f) };
    _streams.Position_id0 = float2(0.0f, 1.0f);
}

void main()
{
    vert_main();
}
