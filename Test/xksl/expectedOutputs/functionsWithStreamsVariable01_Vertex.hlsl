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

static const VS_STREAMS _44 = { 0.0f.xxxx, 0.0f.xxxx, 0, 0.0f };

static float4 VS_IN_S1;
static float4 VS_IN_S2;
static int VS_IN_B1;
static float VS_IN_B2;

struct SPIRV_Cross_Input
{
    int VS_IN_B1 : B1;
    float VS_IN_B2 : B2;
    float4 VS_IN_S1 : S1;
    float4 VS_IN_S2 : S2;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams _17 = { _streams.s1_id0, _streams.s2_id1, _streams.b1_id2, _streams.b2_id3, 0 };
    ShaderMain_Streams res = _17;
    return res;
}

void vert_main()
{
    VS_STREAMS _streams = _44;
    _streams.s1_id0 = VS_IN_S1;
    _streams.s2_id1 = VS_IN_S2;
    _streams.b1_id2 = VS_IN_B1;
    _streams.b2_id3 = VS_IN_B2;
    ShaderMain_Streams backup = ShaderMain__getStreams(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_S1 = stage_input.VS_IN_S1;
    VS_IN_S2 = stage_input.VS_IN_S2;
    VS_IN_B1 = stage_input.VS_IN_B1;
    VS_IN_B2 = stage_input.VS_IN_B2;
    vert_main();
}
