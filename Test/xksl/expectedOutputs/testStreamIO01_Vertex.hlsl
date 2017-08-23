struct VS_STREAMS
{
    float3x3 tangentToWorld_id0;
};

static float3x3 VS_IN_tangentToWorld;
static float3x3 VS_OUT_tangentToWorld;

struct SPIRV_Cross_Input
{
    float3x3 VS_IN_tangentToWorld : TANGENTTOWORLD;
};

struct SPIRV_Cross_Output
{
    float3x3 VS_OUT_tangentToWorld : TANGENTTOWORLD;
};

void vert_main()
{
    VS_STREAMS _streams = { float3x3(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f)) };
    _streams.tangentToWorld_id0 = VS_IN_tangentToWorld;
    VS_OUT_tangentToWorld = _streams.tangentToWorld_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_tangentToWorld[0] = stage_input.VS_IN_tangentToWorld_0;
    VS_IN_tangentToWorld[1] = stage_input.VS_IN_tangentToWorld_1;
    VS_IN_tangentToWorld[2] = stage_input.VS_IN_tangentToWorld_2;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_tangentToWorld = VS_OUT_tangentToWorld;
    return stage_output;
}
