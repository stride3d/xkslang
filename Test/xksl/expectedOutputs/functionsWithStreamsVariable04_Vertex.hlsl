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

static float4 VS_IN_s1;
static float4 VS_IN_b1;

struct SPIRV_Cross_Input
{
    float4 VS_IN_s1 : S1;
    float4 VS_IN_b1 : B1;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams _15 = { _streams.s1_id0, _streams.b1_id1, 0 };
    ShaderMain_Streams res = _15;
    return res;
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxxx };
    _streams.s1_id0 = VS_IN_s1;
    _streams.b1_id1 = VS_IN_b1;
    ShaderMain_Streams s1 = ShaderMain__getStreams(_streams);
    ShaderMain_Streams s2 = ShaderMain__getStreams(_streams);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_s1 = stage_input.VS_IN_s1;
    VS_IN_b1 = stage_input.VS_IN_b1;
    vert_main();
}
