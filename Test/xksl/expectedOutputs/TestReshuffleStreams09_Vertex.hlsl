struct VS_STREAMS
{
    float s1_id0;
    float s3_id1;
    float s4_id2;
};

static const VS_STREAMS _46 = { 0.0f, 0.0f, 0.0f };

static float VS_IN_S4;
static float VS_OUT_s4;

struct SPIRV_Cross_Input
{
    float VS_IN_S4 : S4;
};

struct SPIRV_Cross_Output
{
    float VS_OUT_s4 : S4;
};

void ShaderMain_AnotherFunctionNotDoingAnything()
{
}

float ShaderMain_FunctionUsingStreams(VS_STREAMS _streams)
{
    ShaderMain_AnotherFunctionNotDoingAnything();
    return _streams.s4_id2;
}

void ShaderMain_FunctionNotUsingStreams(VS_STREAMS _streams, int i)
{
    float f = ShaderMain_FunctionUsingStreams(_streams);
}

void ShaderMain_CommonFunctionUsingStreams(inout VS_STREAMS _streams, float f, int i)
{
    _streams.s3_id1 = f;
    int param = i;
    ShaderMain_FunctionNotUsingStreams(_streams, param);
}

void vert_main()
{
    VS_STREAMS _streams = _46;
    _streams.s4_id2 = VS_IN_S4;
    _streams.s1_id0 = 0.0f;
    float param = 1.0f;
    int param_1 = 0;
    ShaderMain_CommonFunctionUsingStreams(_streams, param, param_1);
    VS_OUT_s4 = _streams.s4_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_S4 = stage_input.VS_IN_S4;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_s4 = VS_OUT_s4;
    return stage_output;
}
