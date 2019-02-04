struct ShaderMain_Streams
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

static const VS_STREAMS _34 = { 0.0f.xxxx, 0.0f.xxxx };

static float4 VS_IN_S1;
static float4 VS_IN_B1;

struct SPIRV_Cross_Input
{
    float4 VS_IN_B1 : B1;
    float4 VS_IN_S1 : S1;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams _15 = { _streams.s1_id0, _streams.b1_id1, 0 };
    ShaderMain_Streams res = _15;
    return res;
}

void vert_main()
{
    VS_STREAMS _streams = _34;
    _streams.s1_id0 = VS_IN_S1;
    _streams.b1_id1 = VS_IN_B1;
    ShaderMain_Streams s1 = ShaderMain__getStreams(_streams);
    ShaderMain_Streams s2 = ShaderMain__getStreams(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_S1 = stage_input.VS_IN_S1;
    VS_IN_B1 = stage_input.VS_IN_B1;
    vert_main();
}
