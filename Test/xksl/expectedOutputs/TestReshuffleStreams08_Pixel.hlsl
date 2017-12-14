struct PS_STREAMS
{
    float s2_id0;
    float s3_id1;
};

void ShaderMain_CommonFunctionUsingStreams_PS(inout PS_STREAMS _streams, float f)
{
    _streams.s3_id1 = f;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f, 0.0f };
    _streams.s2_id0 = 0.0f;
    float param = 2.0f;
    ShaderMain_CommonFunctionUsingStreams_PS(_streams, param);
}

void main()
{
    frag_main();
}
