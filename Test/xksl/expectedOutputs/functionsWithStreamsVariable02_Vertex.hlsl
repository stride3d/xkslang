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

static const VS_STREAMS _79 = { 0.0f.xxxx, 0.0f.xxxx, 0, 0.0f };

static int VS_IN_B1;
static float VS_IN_B2;
static float4 VS_OUT_s1;
static float4 VS_OUT_s2;

struct SPIRV_Cross_Input
{
    int VS_IN_B1 : B1;
    float VS_IN_B2 : B2;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_s1 : S1;
    float4 VS_OUT_s2 : S2;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams _23 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain_Streams res = _23;
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
    VS_STREAMS _streams = _79;
    _streams.b1_id2 = VS_IN_B1;
    _streams.b2_id3 = VS_IN_B2;
    _streams.s1_id0 = float4(0.0f, 1.0f, 2.0f, 3.0f);
    _streams.s2_id1 = float4(4.0f, 5.0f, 6.0f, 7.0f);
    ShaderMain_Streams backup = ShaderMain__getStreams(_streams);
    _streams.s2_id1 = float4(8.0f, 9.0f, 10.0f, 11.0f);
    ShaderMain_Streams param = backup;
    ShaderMain__setStreams(_streams, param);
    VS_OUT_s1 = _streams.s1_id0;
    VS_OUT_s2 = _streams.s2_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_B1 = stage_input.VS_IN_B1;
    VS_IN_B2 = stage_input.VS_IN_B2;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_s1 = VS_OUT_s1;
    stage_output.VS_OUT_s2 = VS_OUT_s2;
    return stage_output;
}
