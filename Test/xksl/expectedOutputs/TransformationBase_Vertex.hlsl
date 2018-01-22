struct VS_STREAMS
{
    float4 ShadingPosition_id0;
};

static float4 VS_IN_ShadingPosition;
static float4 VS_OUT_ShadingPosition;

struct SPIRV_Cross_Input
{
    float4 VS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
};

void ShaderBase_VSMain()
{
}

void TransformationBase_PreTransformPosition()
{
}

void TransformationBase_TransformPosition()
{
}

void TransformationBase_PostTransformPosition()
{
}

void TransformationBase_BaseTransformVS()
{
    TransformationBase_PreTransformPosition();
    TransformationBase_TransformPosition();
    TransformationBase_PostTransformPosition();
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxxx };
    _streams.ShadingPosition_id0 = VS_IN_ShadingPosition;
    ShaderBase_VSMain();
    TransformationBase_BaseTransformVS();
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_ShadingPosition = stage_input.VS_IN_ShadingPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    return stage_output;
}
