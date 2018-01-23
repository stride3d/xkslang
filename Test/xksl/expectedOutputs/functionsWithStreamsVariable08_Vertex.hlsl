struct IMaterialStreamBlend_Streams
{
    float matBlend;
    int _unused;
};

struct ShaderMain_Streams
{
    float3 matColor;
    float matBlend;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
    float3 matColor_id1;
};

static float VS_IN_matBlend;
static float3 VS_IN_matColor;

struct SPIRV_Cross_Input
{
    float VS_IN_matBlend : MATBLEND;
    float3 VS_IN_matColor : MATCOLOR;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams _23 = { _streams.matColor_id1, _streams.matBlend_id0, 0 };
    ShaderMain_Streams res = _23;
    return res;
}

IMaterialStreamBlend_Streams ShaderMain__ConvertShaderMainStreamsToIMaterialStreamBlendStreams(ShaderMain_Streams s)
{
    IMaterialStreamBlend_Streams _33 = { s.matBlend, s._unused };
    IMaterialStreamBlend_Streams r = _33;
    return r;
}

void IMaterialStreamBlend_Compute(inout VS_STREAMS _streams, IMaterialStreamBlend_Streams fromStream)
{
    _streams.matBlend_id0 = 0.0f;
}

void ShaderMain_Compute(inout VS_STREAMS _streams, ShaderMain_Streams fromStream)
{
    _streams.matColor_id1 = float3(0.0f, 0.100000001490116119384765625f, 0.20000000298023223876953125f);
    ShaderMain_Streams param = fromStream;
    IMaterialStreamBlend_Streams param_1 = ShaderMain__ConvertShaderMainStreamsToIMaterialStreamBlendStreams(param);
    IMaterialStreamBlend_Compute(_streams, param_1);
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f, 0.0f.xxx };
    _streams.matBlend_id0 = VS_IN_matBlend;
    _streams.matColor_id1 = VS_IN_matColor;
    ShaderMain_Streams param = ShaderMain__getStreams(_streams);
    ShaderMain_Compute(_streams, param);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_matBlend = stage_input.VS_IN_matBlend;
    VS_IN_matColor = stage_input.VS_IN_matColor;
    vert_main();
}
