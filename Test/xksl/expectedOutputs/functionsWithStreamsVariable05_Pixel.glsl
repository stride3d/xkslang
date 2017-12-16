#version 450

struct ShaderMain_Streams
{
    float matBlend;
    int _unused;
};

struct PS_STREAMS
{
    float matBlend_id0;
};

layout(location = 0) in float PS_IN_matBlend;

ShaderMain_Streams ShaderMain__getStreams_PS(PS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.matBlend_id0, 0);
    return res;
}

float ShaderMain_Compute(ShaderMain_Streams fromStream)
{
    return fromStream.matBlend;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0);
    _streams.matBlend_id0 = PS_IN_matBlend;
    ShaderMain_Streams backup = ShaderMain__getStreams_PS(_streams);
    ShaderMain_Streams param = backup;
    float f = ShaderMain_Compute(param);
    ShaderMain_Streams param_1 = ShaderMain__getStreams_PS(_streams);
    float f2 = ShaderMain_Compute(param_1);
}

