#version 450

struct PS_STREAMS
{
    vec4 ScreenPosition_id0;
    float ZDepth_id1;
    vec4 ShadingPosition_id2;
    vec4 ColorTarget_id3;
    vec2 TexCoord_id4;
    vec4 LocalColor_id5;
};

layout(std140) uniform PerMaterial
{
    vec4 ParticleBase_ColorScale;
    float ParticleBase_AlphaAdditive;
    float ParticleBase_ZOffset;
    float ParticleBase_SoftEdgeInverseDistance;
} PerMaterial_var;

layout(std140) uniform PerView
{
    layout(row_major) mat4 ParticleUtilities_ViewMatrix;
    layout(row_major) mat4 ParticleUtilities_ProjectionMatrix;
    layout(row_major) mat4 ParticleUtilities_ViewProjectionMatrix;
    vec4 ParticleUtilities_ViewFrustum;
    vec4 ParticleUtilities_Viewport;
} PerView_var;

uniform sampler2D SPIRV_Cross_CombinedDepthBase_DepthStencilTexturing_PointSampler;

layout(location = 0) in vec4 PS_IN_ScreenPosition;
layout(location = 1) in float PS_IN_ZDepth;
layout(location = 2) in vec4 PS_IN_ShadingPosition;
layout(location = 3) in vec2 PS_IN_TexCoord;
layout(location = 4) in vec4 PS_IN_LocalColor;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec4 ParticleBase_Shading()
{
    return PerMaterial_var.ParticleBase_ColorScale;
}

vec4 o2S302C0_o0S2C0_ComputeColorFromStream_COLOR0_rgba__Compute(PS_STREAMS _streams)
{
    return clamp(_streams.LocalColor_id5, vec4(0.0), vec4(1.0));
}

vec4 o2S302C0_o1S2C1_ComputeColorRadial_float4(PS_STREAMS _streams)
{
    float radialDistance = length(_streams.TexCoord_id4 - vec2(0.5)) * 2.0;
    vec4 unclamped = mix(vec4(1.5, 0.0, 1.5, 1.0), vec4(1.5, 1.5, 0.0, 0.0), vec4(radialDistance));
    vec4 clamped = clamp(unclamped, vec4(0.0), vec4(1000.0, 1000.0, 1000.0, 1.0));
    vec3 _163 = clamped.xyz * clamped.w;
    clamped = vec4(_163.x, _163.y, _163.z, clamped.w);
    return clamped;
}

vec4 o2S302C0_ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    vec4 tex1 = o2S302C0_o0S2C0_ComputeColorFromStream_COLOR0_rgba__Compute(_streams);
    vec4 tex2 = o2S302C0_o1S2C1_ComputeColorRadial_float4(_streams);
    vec4 mix1 = tex1 * tex2;
    return mix1;
}

vec4 ParticleComputeColorShader_Shading(PS_STREAMS _streams)
{
    vec4 finalColor = ParticleBase_Shading() * o2S302C0_ComputeColorMultiply_Compute(_streams);
    return finalColor;
}

float ParticleUtilities_GetLinearDepth(float z)
{
    float fastA = -PerView_var.ParticleUtilities_ProjectionMatrix[2].z;
    float fastB = PerView_var.ParticleUtilities_ProjectionMatrix[3].z;
    return fastB / (z - fastA);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), 0.0, vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0));
    _streams.ScreenPosition_id0 = PS_IN_ScreenPosition;
    _streams.ZDepth_id1 = PS_IN_ZDepth;
    _streams.ShadingPosition_id2 = PS_IN_ShadingPosition;
    _streams.TexCoord_id4 = PS_IN_TexCoord;
    _streams.LocalColor_id5 = PS_IN_LocalColor;
    vec4 colorTarget = ParticleComputeColorShader_Shading(_streams);
    if (false)
    {
        float screenWidth = PerView_var.ParticleUtilities_ViewFrustum.x;
        float screenHeight = PerView_var.ParticleUtilities_ViewFrustum.y;
        vec2 screenCoords = ((_streams.ScreenPosition_id0.xy / _streams.ScreenPosition_id0.ww) * vec2(0.5)) + vec2(0.5);
        screenCoords.y = 1.0 - screenCoords.y;
        screenCoords = PerView_var.ParticleUtilities_Viewport.xy + (screenCoords * PerView_var.ParticleUtilities_Viewport.zw);
        float linearZOwn = -_streams.ZDepth_id1;
        float param = texture(SPIRV_Cross_CombinedDepthBase_DepthStencilTexturing_PointSampler, screenCoords).x;
        float linearZOpaque = ParticleUtilities_GetLinearDepth(param);
        float depthDistance = linearZOpaque - linearZOwn;
        float softEdge = smoothstep(0.0, 1.0, depthDistance * PerMaterial_var.ParticleBase_SoftEdgeInverseDistance);
        colorTarget *= softEdge;
    }
    colorTarget.w *= PerMaterial_var.ParticleBase_AlphaAdditive;
    _streams.ColorTarget_id3 = colorTarget;
    PS_OUT_ColorTarget = _streams.ColorTarget_id3;
}

