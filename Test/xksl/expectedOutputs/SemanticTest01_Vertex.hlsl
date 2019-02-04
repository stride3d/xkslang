struct VS_STREAMS
{
    float4 Position_id0;
    float4 ShadingPosition_id1;
};

static const VS_STREAMS _28 = { 0.0f.xxxx, 0.0f.xxxx };

cbuffer PerDraw
{
    column_major float4x4 ShaderMain_MatrixTransform;
};

static float4 gl_Position;
static float4 VS_IN_POSITION;

struct SPIRV_Cross_Input
{
    float4 VS_IN_POSITION : POSITION;
};

struct SPIRV_Cross_Output
{
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    VS_STREAMS _streams = _28;
    _streams.Position_id0 = VS_IN_POSITION;
    _streams.ShadingPosition_id1 = mul(_streams.Position_id0, ShaderMain_MatrixTransform);
    gl_Position = _streams.ShadingPosition_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_POSITION = stage_input.VS_IN_POSITION;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    return stage_output;
}
