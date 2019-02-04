#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    float s2_id0;
    float s3_id1;
    float s4_id2;
};

in float PS_IN_S4;

void ShaderMain_AnotherFunctionNotDoingAnything()
{
}

float ShaderMain_FunctionUsingStreams_PS(PS_STREAMS _streams)
{
    ShaderMain_AnotherFunctionNotDoingAnything();
    return _streams.s4_id2;
}

void ShaderMain_FunctionNotUsingStreams_PS(PS_STREAMS _streams, int i)
{
    float f = ShaderMain_FunctionUsingStreams_PS(_streams);
}

void ShaderMain_CommonFunctionUsingStreams_PS(inout PS_STREAMS _streams, float f, int i)
{
    _streams.s3_id1 = f;
    int param = i;
    ShaderMain_FunctionNotUsingStreams_PS(_streams, param);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0.0, 0.0);
    _streams.s4_id2 = PS_IN_S4;
    _streams.s2_id0 = 0.0;
    float param = 2.0;
    int param_1 = 1;
    ShaderMain_CommonFunctionUsingStreams_PS(_streams, param, param_1);
}

