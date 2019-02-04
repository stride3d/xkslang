struct VS_STREAMS
{
    float3x3 tangentToWorld_id0;
};

static const VS_STREAMS _21 = { float3x3(0.0f.xxx, 0.0f.xxx, 0.0f.xxx) };

static float3x3 VS_IN_TANGENTTOWORLD;
static float3x3 VS_OUT_tangentToWorld;

struct SPIRV_Cross_Input
{
    float3x3 VS_IN_TANGENTTOWORLD : TANGENTTOWORLD;
};

struct SPIRV_Cross_Output
{
    float3x3 VS_OUT_tangentToWorld : TANGENTTOWORLD;
};

void vert_main()
{
    VS_STREAMS _streams = _21;
    _streams.tangentToWorld_id0 = VS_IN_TANGENTTOWORLD;
    VS_OUT_tangentToWorld = _streams.tangentToWorld_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TANGENTTOWORLD[0] = stage_input.VS_IN_TANGENTTOWORLD_0;
    VS_IN_TANGENTTOWORLD[1] = stage_input.VS_IN_TANGENTTOWORLD_1;
    VS_IN_TANGENTTOWORLD[2] = stage_input.VS_IN_TANGENTTOWORLD_2;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_tangentToWorld = VS_OUT_tangentToWorld;
    return stage_output;
}
