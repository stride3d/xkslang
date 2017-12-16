struct ShaderMain_Streams
{
    float3 matColor;
    float matBlend;
    float3 matNormal;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
    float3 matNormal_id1;
    float3 matColor_id2;
};

static float VS_IN_matBlend;
static float3 VS_IN_matNormal;
static float3 VS_IN_matColor;

struct SPIRV_Cross_Input
{
    float VS_IN_matBlend : MATBLEND;
    float3 VS_IN_matNormal : MATNORMAL;
    float3 VS_IN_matColor : MATCOLOR;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = { _streams.matColor_id2, _streams.matBlend_id0, _streams.matNormal_id1, 0 };
    return res;
}

void ShaderMain_Compute(inout VS_STREAMS _streams, ShaderMain_Streams fromStream)
{
    float3 middleNormal = (fromStream.matNormal + _streams.matNormal_id1) + _streams.matColor_id2;
    float3 _6;
    if (_streams.matBlend_id0 < 0.5f)
    {
        _6 = lerp(fromStream.matNormal, middleNormal, (_streams.matBlend_id0 / 0.5f).xxx);
    }
    else
    {
        _6 = lerp(middleNormal, _streams.matNormal_id1, ((_streams.matBlend_id0 - 0.5f) * 2.0f).xxx);
    }
    _streams.matNormal_id1 = _6;
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f) };
    _streams.matBlend_id0 = VS_IN_matBlend;
    _streams.matNormal_id1 = VS_IN_matNormal;
    _streams.matColor_id2 = VS_IN_matColor;
    ShaderMain_Streams param = ShaderMain__getStreams(_streams);
    ShaderMain_Compute(_streams, param);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_matBlend = stage_input.VS_IN_matBlend;
    VS_IN_matNormal = stage_input.VS_IN_matNormal;
    VS_IN_matColor = stage_input.VS_IN_matColor;
    vert_main();
}
