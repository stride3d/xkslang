struct VS_STREAMS
{
    float4 ShadingPosition_id0;
};

static const VS_STREAMS _34 = { 0.0f.xxxx };

static float4 gl_Position;
static float4 VS_IN_SV_Position;

struct SPIRV_Cross_Input
{
    float4 VS_IN_SV_Position : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 gl_Position : SV_Position;
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
    VS_STREAMS _streams = _34;
    _streams.ShadingPosition_id0 = VS_IN_SV_Position;
    ShaderBase_VSMain();
    TransformationBase_BaseTransformVS();
    gl_Position = _streams.ShadingPosition_id0;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_SV_Position = stage_input.VS_IN_SV_Position;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    return stage_output;
}
