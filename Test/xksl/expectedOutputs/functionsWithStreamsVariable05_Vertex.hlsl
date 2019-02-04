struct ShaderMain_Streams
{
    float matBlend;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
};

static const VS_STREAMS _42 = { 0.0f };

static float VS_IN_MATBLEND;
static float VS_OUT_matBlend;

struct SPIRV_Cross_Input
{
    float VS_IN_MATBLEND : MATBLEND;
};

struct SPIRV_Cross_Output
{
    float VS_OUT_matBlend : MATBLEND;
};

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams _24 = { _streams.matBlend_id0, 0 };
    ShaderMain_Streams res = _24;
    return res;
}

float ShaderMain_Compute(ShaderMain_Streams fromStream)
{
    return fromStream.matBlend;
}

void vert_main()
{
    VS_STREAMS _streams = _42;
    _streams.matBlend_id0 = VS_IN_MATBLEND;
    ShaderMain_Streams backup = ShaderMain__getStreams(_streams);
    ShaderMain_Streams param = backup;
    float f = ShaderMain_Compute(param);
    ShaderMain_Streams param_1 = ShaderMain__getStreams(_streams);
    float f2 = ShaderMain_Compute(param_1);
    VS_OUT_matBlend = _streams.matBlend_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_MATBLEND = stage_input.VS_IN_MATBLEND;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_matBlend = VS_OUT_matBlend;
    return stage_output;
}
