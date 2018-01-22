struct VS_STREAMS
{
    float4 outStreamA_id0;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx };
    _streams.outStreamA_id0 = 5.0f.xxxx;
}

void main()
{
    vert_main();
}
