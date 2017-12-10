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

struct SPIRV_Cross_Input
{
    float4 VS_IN_s1 : S1;
    float4 VS_IN_s2 : S2;
    int VS_IN_b1 : B1;
    float VS_IN_b2 : B2;
};

ShaderMain_Streams ShaderMain__getStreamsStructType(VS_STREAMS _streams)
{
    ShaderMain_Streams res = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    return res;
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), 0, 0.0f };
    _streams.s1_id0 = VS_IN_s1;
    _streams.s2_id1 = VS_IN_s2;
    _streams.b1_id2 = VS_IN_b1;
    _streams.b2_id3 = VS_IN_b2;
    ShaderMain_Streams backup = ShaderMain__getStreamsStructType(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_s1 = stage_input.VS_IN_s1;
    VS_IN_s2 = stage_input.VS_IN_s2;
    VS_IN_b1 = stage_input.VS_IN_b1;
    VS_IN_b2 = stage_input.VS_IN_b2;
    vert_main();
}
