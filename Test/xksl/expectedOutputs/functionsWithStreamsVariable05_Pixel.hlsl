struct ShaderMain_Streams
{
    float matBlend;
    int _unused;
};

struct PS_STREAMS
{
    float matBlend_id0;
};

static float PS_IN_matBlend;

struct SPIRV_Cross_Input
{
    float PS_IN_matBlend : MATBLEND;
};

ShaderMain_Streams ShaderMain__getStreams_PS(PS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams{ _streams.matBlend_id0, 0 };
    return res;
}

float ShaderMain_Compute(ShaderMain_Streams fromStream)
{
    return fromStream.matBlend;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f };
    _streams.matBlend_id0 = PS_IN_matBlend;
    ShaderMain_Streams backup = ShaderMain__getStreams_PS(_streams);
    ShaderMain_Streams param = backup;
    float f = ShaderMain_Compute(param);
    ShaderMain_Streams param_1 = ShaderMain__getStreams_PS(_streams);
    float f2 = ShaderMain_Compute(param_1);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_matBlend = stage_input.PS_IN_matBlend;
    frag_main();
}
