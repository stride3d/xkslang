struct ShaderMain__streamsStruct
{
    float matBlend;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
};

static float VS_IN_matBlend;

struct SPIRV_Cross_Input
{
    float VS_IN_matBlend : MATBLEND;
};

float ShaderMain_Compute(ShaderMain__streamsStruct fromStream)
{
    return fromStream.matBlend;
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f };
    _streams.matBlend_id0 = VS_IN_matBlend;
    ShaderMain__streamsStruct backup = { _streams.matBlend_id0, 0 };
    ShaderMain__streamsStruct param = backup;
    float f = ShaderMain_Compute(param);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_matBlend = stage_input.VS_IN_matBlend;
    vert_main();
}
