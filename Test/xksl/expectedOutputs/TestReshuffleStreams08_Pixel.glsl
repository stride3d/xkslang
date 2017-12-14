#version 450

struct PS_STREAMS
{
    float s2_id0;
    float s3_id1;
};

void ShaderMain_CommonFunctionUsingStreams_PS(inout PS_STREAMS _streams, float f)
{
    _streams.s3_id1 = f;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0.0);
    _streams.s2_id0 = 0.0;
    float param = 2.0;
    ShaderMain_CommonFunctionUsingStreams_PS(_streams, param);
}

