struct VS_STREAMS
{
    float aStream1_id0;
    float aStream2_id1;
};

cbuffer PerLighting
{
    float ShaderMain_var1;
    float ShaderMain_var2;
};

static float VS_IN_aStream1;
static float VS_OUT_aStream2;

struct SPIRV_Cross_Input
{
    float VS_IN_aStream1 : STREAM1;
};

struct SPIRV_Cross_Output
{
    float VS_OUT_aStream2 : TEXCOORD0;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f, 0.0f };
    _streams.aStream1_id0 = VS_IN_aStream1;
    _streams.aStream2_id1 = _streams.aStream1_id0 + ShaderMain_var1;
    VS_OUT_aStream2 = _streams.aStream2_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_aStream1 = stage_input.VS_IN_aStream1;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStream2 = VS_OUT_aStream2;
    return stage_output;
}
