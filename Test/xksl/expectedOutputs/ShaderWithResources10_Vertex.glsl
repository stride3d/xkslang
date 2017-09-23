#version 450

struct VS_STREAMS
{
    int lightIndex_id0;
};

uniform usamplerBuffer ShaderMain_LightIndices;

layout(location = 0) in int VS_IN_lightIndex;

void ShaderMain_PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(texelFetch(ShaderMain_LightIndices, _streams.lightIndex_id0).x);
    _streams.lightIndex_id0++;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0);
    _streams.lightIndex_id0 = VS_IN_lightIndex;
    int param = 0;
    ShaderMain_PrepareDirectLightCore(_streams, param);
}

