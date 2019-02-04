#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

in float VS_IN_MATBLEND;
in vec3 VS_IN_MATNORMAL;
in vec3 VS_IN_MATCOLOR;

ShaderMain_Streams ShaderMain__getStreams(VS_STREAMS _streams)
{
    ShaderMain_Streams res = ShaderMain_Streams(_streams.matColor_id2, _streams.matBlend_id0, _streams.matNormal_id1, 0);
    return res;
}

void ShaderMain_Compute(inout VS_STREAMS _streams, ShaderMain_Streams fromStream)
{
    vec3 middleNormal = (fromStream.matNormal + _streams.matNormal_id1) + _streams.matColor_id2;
    vec3 _24 = mix(fromStream.matNormal, middleNormal, vec3(_streams.matBlend_id0 / 0.5));
    vec3 _33 = mix(middleNormal, _streams.matNormal_id1, vec3((_streams.matBlend_id0 - 0.5) * 2.0));
    bvec3 _34 = bvec3(_streams.matBlend_id0 < 0.5);
    _streams.matNormal_id1 = vec3(_34.x ? _24.x : _33.x, _34.y ? _24.y : _33.y, _34.z ? _24.z : _33.z);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, vec3(0.0), vec3(0.0));
    _streams.matBlend_id0 = VS_IN_MATBLEND;
    _streams.matNormal_id1 = VS_IN_MATNORMAL;
    _streams.matColor_id2 = VS_IN_MATCOLOR;
    ShaderMain_Streams param = ShaderMain__getStreams(_streams);
    ShaderMain_Compute(_streams, param);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

