struct VS_STREAMS
{
    float4 Position_id0;
    float4 ShadingPosition_id1;
};

static const VS_STREAMS _47 = { 0.0f.xxxx, 0.0f.xxxx };

cbuffer PerDraw
{
    column_major float4x4 ShaderMain_MatrixTransform;
    column_major float4x3 ShaderMain_Matrix4_3;
    column_major float2x3 ShaderMain_Matrix2_3;
    float3 ShaderMain_ColorMatrix;
    float4 ShaderMain_ColorArray[2];
};
cbuffer Globals
{
    float ShaderMain_depthOffset;
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
    VS_STREAMS _streams = _47;
    _streams.Position_id0 = VS_IN_POSITION;
    _streams.ShadingPosition_id1 = mul(_streams.Position_id0, ShaderMain_MatrixTransform) + float4(ShaderMain_ColorArray[1].xyz, ShaderMain_depthOffset);
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
