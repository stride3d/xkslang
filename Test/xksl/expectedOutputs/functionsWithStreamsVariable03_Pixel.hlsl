struct ShaderMain__streamsStruct
{
    float4 s1;
    float4 s2;
    int b1;
    float b2;
    int _unused;
};

struct PS_STREAMS
{
    float4 s1_id0;
    float4 s2_id1;
    int b1_id2;
    float b2_id3;
};

static float4 PS_IN_s1;
static float4 PS_IN_s2;
static int PS_IN_b1;
static float PS_IN_b2;

struct SPIRV_Cross_Input
{
    float4 PS_IN_s1 : S1;
    float4 PS_IN_s2 : S2;
    int PS_IN_b1 : B1;
    float PS_IN_b2 : B2;
};

void frag_main()
{
    PS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0, 0.0f };
    _streams.s1_id0 = PS_IN_s1;
    _streams.s2_id1 = PS_IN_s2;
    _streams.b1_id2 = PS_IN_b1;
    _streams.b2_id3 = PS_IN_b2;
    ShaderMain__streamsStruct s1 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain__streamsStruct s3 = s1;
    ShaderMain__streamsStruct s6 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain__streamsStruct s2 = s3;
    ShaderMain__streamsStruct _tmpStreamsVar_12 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain__streamsStruct s4 = _tmpStreamsVar_12;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_s1 = stage_input.PS_IN_s1;
    PS_IN_s2 = stage_input.PS_IN_s2;
    PS_IN_b1 = stage_input.PS_IN_b1;
    PS_IN_b2 = stage_input.PS_IN_b2;
    frag_main();
}
