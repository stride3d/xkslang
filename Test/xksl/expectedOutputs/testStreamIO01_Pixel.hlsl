struct PS_STREAMS
{
    float3x3 tangentToWorld_id0;
};

static const PS_STREAMS _22 = { float3x3(0.0f.xxx, 0.0f.xxx, 0.0f.xxx) };

static float3x3 PS_IN_TANGENTTOWORLD;

struct SPIRV_Cross_Input
{
    float3x3 PS_IN_TANGENTTOWORLD : TANGENTTOWORLD;
};

void frag_main()
{
    PS_STREAMS _streams = _22;
    _streams.tangentToWorld_id0 = PS_IN_TANGENTTOWORLD;
    float3x3 v2 = _streams.tangentToWorld_id0;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TANGENTTOWORLD = stage_input.PS_IN_TANGENTTOWORLD;
    frag_main();
}
