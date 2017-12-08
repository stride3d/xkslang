#version 450

struct ShaderMain__streamsStruct
{
    float matBlend;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
};

layout(location = 0) in float VS_IN_matBlend;

float ShaderMain_Compute(ShaderMain__streamsStruct fromStream)
{
    return fromStream.matBlend;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0);
    _streams.matBlend_id0 = VS_IN_matBlend;
    ShaderMain__streamsStruct backup = ShaderMain__streamsStruct(_streams.matBlend_id0, 0);
    ShaderMain__streamsStruct param = backup;
    float f = ShaderMain_Compute(param);
}

