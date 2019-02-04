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

static const VS_STREAMS _78 = { 0.0f, 0.0f.xxx, 0.0f.xxx };

static float VS_IN_MATBLEND;
static float3 VS_IN_MATNORMAL;
static float3 VS_IN_MATCOLOR;

struct SPIRV_Cross_Input
{
    float VS_IN_MATBLEND : MATBLEND;
    float3 VS_IN_MATCOLOR : MATCOLOR;
    float3 VS_IN_MATNORMAL : MATNORMAL;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams _51 = { _streams.matColor_id2, _streams.matBlend_id0, _streams.matNormal_id1, 0 };
    ShaderMain_Streams res = _51;
    return res;
}

void ShaderMain_Compute(inout VS_STREAMS _streams, ShaderMain_Streams fromStream)
{
    float3 middleNormal = (fromStream.matNormal + _streams.matNormal_id1) + _streams.matColor_id2;
    float3 _24 = lerp(fromStream.matNormal, middleNormal, (_streams.matBlend_id0 / 0.5f).xxx);
    float3 _33 = lerp(middleNormal, _streams.matNormal_id1, ((_streams.matBlend_id0 - 0.5f) * 2.0f).xxx);
    bool3 _34 = (_streams.matBlend_id0 < 0.5f).xxx;
    _streams.matNormal_id1 = float3(_34.x ? _24.x : _33.x, _34.y ? _24.y : _33.y, _34.z ? _24.z : _33.z);
}

void vert_main()
{
    VS_STREAMS _streams = _78;
    _streams.matBlend_id0 = VS_IN_MATBLEND;
    _streams.matNormal_id1 = VS_IN_MATNORMAL;
    _streams.matColor_id2 = VS_IN_MATCOLOR;
    ShaderMain_Streams param = ShaderMain__getStreams(_streams);
    ShaderMain_Compute(_streams, param);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_MATBLEND = stage_input.VS_IN_MATBLEND;
    VS_IN_MATNORMAL = stage_input.VS_IN_MATNORMAL;
    VS_IN_MATCOLOR = stage_input.VS_IN_MATCOLOR;
    vert_main();
}
