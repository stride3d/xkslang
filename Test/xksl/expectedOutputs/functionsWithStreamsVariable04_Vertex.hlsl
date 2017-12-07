struct ShaderMain__streamsStruct
{
    float4 s1;
    float4 b1;
    int _unused;
};

struct VS_STREAMS
{
    float4 s1_id0;
    float4 b1_id1;
};

static float4 VS_IN_s1;
static float4 VS_IN_b1;

struct SPIRV_Cross_Input
{
    float4 VS_IN_s1 : S1;
    float4 VS_IN_b1 : B1;
};

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.s1_id0 = VS_IN_s1;
    _streams.b1_id1 = VS_IN_b1;
    ShaderMain__streamsStruct s1 = { _streams.s1_id0, _streams.b1_id1, 0 };
    ShaderMain__streamsStruct _tmpStreamsVar_2 = { _streams.s1_id0, _streams.b1_id1, 0 };
    ShaderMain__streamsStruct s2 = _tmpStreamsVar_2;
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_s1 = stage_input.VS_IN_s1;
    VS_IN_b1 = stage_input.VS_IN_b1;
    vert_main();
}
