struct VS_STREAMS
{
    float4 Position_id0;
    float4 ShadingPosition_id1;
};

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

static float4 VS_IN_Position;
static float4 VS_OUT_ShadingPosition;

struct SPIRV_Cross_Input
{
    float4 VS_IN_Position : POSITION;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx, 0.0f.xxxx };
    _streams.Position_id0 = VS_IN_Position;
    _streams.ShadingPosition_id1 = mul(_streams.Position_id0, ShaderMain_MatrixTransform) + float4(ShaderMain_ColorArray[1].xyz, ShaderMain_depthOffset);
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_Position = stage_input.VS_IN_Position;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    return stage_output;
}
