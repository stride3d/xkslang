struct VS_STREAMS
{
    float4 outStreamA_id0;
};

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.outStreamA_id0 = float4(5.0f, 5.0f, 5.0f, 5.0f);
}

void main()
{
    vert_main();
}
