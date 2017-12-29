#version 450

struct IMaterialStreamBlend_Streams
{
    float matBlend;
    int _unused;
};

struct ShaderMain_Streams
{
    vec3 matColor;
    float matBlend;
    int _unused;
};

struct VS_STREAMS
{
    float matBlend_id0;
    vec3 matColor_id1;
};

layout(location = 0) in float VS_IN_matBlend;
layout(location = 1) in vec3 VS_IN_matColor;

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.matColor_id1, _streams.matBlend_id0, 0);
    return res;
}

IMaterialStreamBlend_Streams ShaderMain__ConvertShaderMainStreamsToIMaterialStreamBlendStreams(ShaderMain_Streams s)
{
    IMaterialStreamBlend_Streams r = IMaterialStreamBlend_Streams(s.matBlend, s._unused);
    return r;
}

void IMaterialStreamBlend_Compute(inout VS_STREAMS _streams, IMaterialStreamBlend_Streams fromStream)
{
    _streams.matBlend_id0 = 0.0;
}

void ShaderMain_Compute(inout VS_STREAMS _streams, ShaderMain_Streams fromStream)
{
    _streams.matColor_id1 = vec3(0.0, 0.100000001490116119384765625, 0.20000000298023223876953125);
    ShaderMain_Streams param = fromStream;
    IMaterialStreamBlend_Streams param_1 = ShaderMain__ConvertShaderMainStreamsToIMaterialStreamBlendStreams(param);
    IMaterialStreamBlend_Compute(_streams, param_1);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, vec3(0.0));
    _streams.matBlend_id0 = VS_IN_matBlend;
    _streams.matColor_id1 = VS_IN_matColor;
    ShaderMain_Streams param = ShaderMain__getStreams(_streams);
    ShaderMain_Compute(_streams, param);
}

