struct VS_STREAMS
{
    int lightIndex_id0;
};

Buffer<uint4> ShaderMain_LightIndices;

static int VS_IN_lightIndex;

struct SPIRV_Cross_Input
{
    int VS_IN_lightIndex : LIGHTINDEX;
};

void ShaderMain_PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(ShaderMain_LightIndices.Load(_streams.lightIndex_id0).x);
    _streams.lightIndex_id0++;
}

void vert_main()
{
    VS_STREAMS _streams = { 0 };
    _streams.lightIndex_id0 = VS_IN_lightIndex;
    int param = 0;
    ShaderMain_PrepareDirectLightCore(_streams, param);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_lightIndex = stage_input.VS_IN_lightIndex;
    vert_main();
}
