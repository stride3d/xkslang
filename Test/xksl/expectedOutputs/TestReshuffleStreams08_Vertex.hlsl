struct VS_STREAMS
{
    float s1_id0;
    float s3_id1;
};

static const VS_STREAMS _23 = { 0.0f, 0.0f };

void ShaderMain_CommonFunctionUsingStreams(inout VS_STREAMS _streams, float f)
{
    _streams.s3_id1 = f;
}

void vert_main()
{
    VS_STREAMS _streams = _23;
    _streams.s1_id0 = 0.0f;
    float param = 1.0f;
    ShaderMain_CommonFunctionUsingStreams(_streams, param);
}

void main()
{
    vert_main();
}
