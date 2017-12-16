#version 450

struct ShaderMain_Streams
{
    vec3 matColor;
    float matBlend;
    vec3 matNormal;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
    vec3 matNormal_id1;
    vec3 matColor_id2;
};

layout(location = 0) in float VS_IN_matBlend;
layout(location = 1) in vec3 VS_IN_matNormal;
layout(location = 2) in vec3 VS_IN_matColor;

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.matColor_id2, _streams.matBlend_id0, _streams.matNormal_id1, 0);
    return res;
}

void ShaderMain_Compute(inout VS_STREAMS _streams, ShaderMain_Streams fromStream)
{
    vec3 middleNormal = (fromStream.matNormal + _streams.matNormal_id1) + _streams.matColor_id2;
    vec3 _6;
    if (_streams.matBlend_id0 < 0.5)
    {
        _6 = mix(fromStream.matNormal, middleNormal, vec3(_streams.matBlend_id0 / 0.5));
    }
    else
    {
        _6 = mix(middleNormal, _streams.matNormal_id1, vec3((_streams.matBlend_id0 - 0.5) * 2.0));
    }
    _streams.matNormal_id1 = _6;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, vec3(0.0), vec3(0.0));
    _streams.matBlend_id0 = VS_IN_matBlend;
    _streams.matNormal_id1 = VS_IN_matNormal;
    _streams.matColor_id2 = VS_IN_matColor;
    ShaderMain_Streams param = ShaderMain__getStreams(_streams);
    ShaderMain_Compute(_streams, param);
}

