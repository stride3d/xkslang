#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 ShadingPosition_id0;
};

in vec4 VS_IN_SV_Position;

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

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.ShadingPosition_id0 = VS_IN_SV_Position;
    ShaderBase_VSMain();
    TransformationBase_BaseTransformVS();
    gl_Position = _streams.ShadingPosition_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

