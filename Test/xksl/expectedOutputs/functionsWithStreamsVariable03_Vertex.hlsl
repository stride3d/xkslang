struct ShaderMain_Streams
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
static float4 VS_OUT_s1;
static float4 VS_OUT_s2;
static int VS_OUT_b1;
static float VS_OUT_b2;

struct SPIRV_Cross_Input
{
    float4 VS_IN_s1 : S1;
    float4 VS_IN_s2 : S2;
    int VS_IN_b1 : B1;
    float VS_IN_b2 : B2;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_s1 : S1;
    float4 VS_OUT_s2 : S2;
    int VS_OUT_b1 : B1;
    float VS_OUT_b2 : B2;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    return res;
}

void ShaderMain__setStreams(inout VS_STREAMS _streams, ShaderMain_Streams _s)
{
    _streams.s1_id0 = _s.s1;
    _streams.s2_id1 = _s.s2;
    _streams.b1_id2 = _s.b1;
    _streams.b2_id3 = _s.b2;
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0, 0.0f };
    _streams.s1_id0 = VS_IN_s1;
    _streams.s2_id1 = VS_IN_s2;
    _streams.b1_id2 = VS_IN_b1;
    _streams.b2_id3 = VS_IN_b2;
    ShaderMain_Streams s1 = ShaderMain__getStreams(_streams);
    ShaderMain_Streams s3 = s1;
    ShaderMain_Streams s6 = ShaderMain__getStreams(_streams);
    ShaderMain_Streams s2 = s3;
    ShaderMain_Streams s4 = ShaderMain__getStreams(_streams);
    ShaderMain_Streams _21 = ShaderMain__getStreams(_streams);
    s2 = _21;
    ShaderMain_Streams s5 = _21;
    ShaderMain_Streams param = s1;
    ShaderMain__setStreams(_streams, param);
    ShaderMain_Streams backup1 = ShaderMain__getStreams(_streams);
    ShaderMain_Streams backup2 = backup1;
    ShaderMain_Streams backup3 = backup2;
    ShaderMain_Streams param_1 = backup3;
    ShaderMain__setStreams(_streams, param_1);
    ShaderMain_Streams backup4 = ShaderMain__getStreams(_streams);
    VS_OUT_s1 = _streams.s1_id0;
    VS_OUT_s2 = _streams.s2_id1;
    VS_OUT_b1 = _streams.b1_id2;
    VS_OUT_b2 = _streams.b2_id3;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_s1 = stage_input.VS_IN_s1;
    VS_IN_s2 = stage_input.VS_IN_s2;
    VS_IN_b1 = stage_input.VS_IN_b1;
    VS_IN_b2 = stage_input.VS_IN_b2;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_s1 = VS_OUT_s1;
    stage_output.VS_OUT_s2 = VS_OUT_s2;
    stage_output.VS_OUT_b1 = VS_OUT_b1;
    stage_output.VS_OUT_b2 = VS_OUT_b2;
    return stage_output;
}
