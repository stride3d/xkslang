struct PS_STREAMS
{
    float3x3 tangentToWorld_id0;
};

static float3x3 PS_IN_tangentToWorld;

struct SPIRV_Cross_Input
{
    float3x3 PS_IN_tangentToWorld : TANGENTTOWORLD;
};

void frag_main()
{
    PS_STREAMS _streams = { float3x3(0.0f.xxx, 0.0f.xxx, 0.0f.xxx) };
    _streams.tangentToWorld_id0 = PS_IN_tangentToWorld;
    float3x3 v2 = _streams.tangentToWorld_id0;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_tangentToWorld = stage_input.PS_IN_tangentToWorld;
    frag_main();
}
