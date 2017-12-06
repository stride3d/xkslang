struct ShaderMain__streamsStruct
{
    float4 s1;
    float4 s2;
    int b1;
    float b2;
    int _unused;
};

struct VS_STREAMS
{
    float4 s1_id0;
    float4 s2_id1;
    int b1_id2;
    float b2_id3;
};

static float4 VS_IN_s1;
static float4 VS_IN_s2;
static int VS_IN_b1;
static float VS_IN_b2;

struct SPIRV_Cross_Input
{
    float4 VS_IN_s1 : S1;
    float4 VS_IN_s2 : S2;
    int VS_IN_b1 : B1;
    float VS_IN_b2 : B2;
};

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0, 0.0f };
    _streams.s1_id0 = VS_IN_s1;
    _streams.s2_id1 = VS_IN_s2;
    _streams.b1_id2 = VS_IN_b1;
    _streams.b2_id3 = VS_IN_b2;
    ShaderMain__streamsStruct s1 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain__streamsStruct s3 = s1;
    ShaderMain__streamsStruct s6 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain__streamsStruct s2 = s3;
    ShaderMain__streamsStruct _tmpStreamsVar_3 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain__streamsStruct s4 = _tmpStreamsVar_3;
    ShaderMain__streamsStruct s5 = s2;
    ShaderMain__streamsStruct _tmpStreamsVar_5 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    s2 = _tmpStreamsVar_5;
    _streams.s1_id0 = s1.s1;
    _streams.s2_id1 = s1.s2;
    _streams.b1_id2 = s1.b1;
    _streams.b2_id3 = s1.b2;
    ShaderMain__streamsStruct backup1 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain__streamsStruct backup2;
    backup2.s1 = backup1.s1;
    backup2.s2 = backup1.s2;
    backup2.b1 = backup1.b1;
    backup2.b2 = backup1.b2;
    backup2._unused = backup1._unused;
    ShaderMain__streamsStruct backup3 = backup2;
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_s1 = stage_input.VS_IN_s1;
    VS_IN_s2 = stage_input.VS_IN_s2;
    VS_IN_b1 = stage_input.VS_IN_b1;
    VS_IN_b2 = stage_input.VS_IN_b2;
    vert_main();
}
