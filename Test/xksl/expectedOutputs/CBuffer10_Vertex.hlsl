struct VS_STREAMS
{
    float aStream1_id0;
    float aStream2_id1;
};

static const VS_STREAMS _25 = { 0.0f, 0.0f };

cbuffer PerLighting
{
    float ShaderMain_var1;
    float ShaderMain_var2;
};

static float VS_IN_STREAM1;
static float VS_OUT_aStream2;

struct SPIRV_Cross_Input
{
    float VS_IN_STREAM1 : STREAM1;
};

struct SPIRV_Cross_Output
{
    float VS_OUT_aStream2 : ASTREAM2;
};

void vert_main()
{
    VS_STREAMS _streams = _25;
    _streams.aStream1_id0 = VS_IN_STREAM1;
    _streams.aStream2_id1 = _streams.aStream1_id0 + ShaderMain_var1;
    VS_OUT_aStream2 = _streams.aStream2_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_STREAM1 = stage_input.VS_IN_STREAM1;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream2 = VS_OUT_aStream2;
    return stage_output;
}
