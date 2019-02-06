#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 ShadingPosition_id0;
    vec4 ColorTarget_id1;
    vec3 viewWS_id2;
    vec4 PositionWS_id3;
    vec3 shadingColor_id4;
    float shadingColorAlpha_id5;
};

layout(std140) uniform PerView
{
    layout(row_major) mat4 Transformation_View;
    layout(row_major) mat4 Transformation_ViewInverse;
    layout(row_major) mat4 Transformation_Projection;
    layout(row_major) mat4 Transformation_ProjectionInverse;
    layout(row_major) mat4 Transformation_ViewProjection;
    vec2 Transformation_ProjScreenRay;
    vec4 Transformation_Eye;
} PerView_var;

in vec4 PS_IN_SV_Position;
in vec4 PS_IN_POSITION_WS;
in float PS_IN_SHADINGCOLORALPHA;
out vec4 PS_OUT_ColorTarget;

void ShaderBase_PSMain()
{
}

void o3S41C1_IStreamInitializer_ResetStream()
{
}

void o2S41C0_IMaterialSurface_Compute()
{
}

vec4 MaterialSurfacePixelStageCompositor_Shading(inout PS_STREAMS _streams)
{
    _streams.viewWS_id2 = normalize(PerView_var.Transformation_Eye.xyz - _streams.PositionWS_id3.xyz);
    _streams.shadingColor_id4 = vec3(0.0);
    o3S41C1_IStreamInitializer_ResetStream();
    o2S41C0_IMaterialSurface_Compute();
    return vec4(_streams.shadingColor_id4, _streams.shadingColorAlpha_id5);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec3(0.0), 0.0);
    _streams.ShadingPosition_id0 = PS_IN_SV_Position;
    _streams.PositionWS_id3 = PS_IN_POSITION_WS;
    _streams.shadingColorAlpha_id5 = PS_IN_SHADINGCOLORALPHA;
    ShaderBase_PSMain();
    vec4 _36 = MaterialSurfacePixelStageCompositor_Shading(_streams);
    _streams.ColorTarget_id1 = _36;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}

