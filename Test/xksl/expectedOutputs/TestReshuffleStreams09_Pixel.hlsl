struct PS_STREAMS
{
    float s2_id0;
    float s3_id1;
    float s4_id2;
};

static const PS_STREAMS _44 = { 0.0f, 0.0f, 0.0f };

static float PS_IN_S4;

struct SPIRV_Cross_Input
{
    float PS_IN_S4 : S4;
};

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

void frag_main()
{
    PS_STREAMS _streams = _44;
    _streams.s4_id2 = PS_IN_S4;
    _streams.s2_id0 = 0.0f;
    float param = 2.0f;
    int param_1 = 1;
    ShaderMain_CommonFunctionUsingStreams_PS(_streams, param, param_1);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_S4 = stage_input.PS_IN_S4;
    frag_main();
}
