struct PS_STREAMS
{
    float4 ScreenPosition_id0;
    float ZDepth_id1;
    float4 ShadingPosition_id2;
    float4 ColorTarget_id3;
    float2 TexCoord_id4;
    float4 LocalColor_id5;
};

cbuffer PerMaterial
{
    float4 ParticleBase_ColorScale;
    float ParticleBase_AlphaAdditive;
    float ParticleBase_ZOffset;
    float ParticleBase_SoftEdgeInverseDistance;
};
cbuffer PerView
{
    column_major float4x4 ParticleUtilities_ViewMatrix;
    column_major float4x4 ParticleUtilities_ProjectionMatrix;
    column_major float4x4 ParticleUtilities_ViewProjectionMatrix;
    float4 ParticleUtilities_ViewFrustum;
    float4 ParticleUtilities_Viewport;
};
Texture2D<float4> DepthBase_DepthStencil;
SamplerState Texturing_PointSampler;

static float4 PS_IN_ScreenPosition;
static float PS_IN_ZDepth;
static float4 PS_IN_ShadingPosition;
static float2 PS_IN_TexCoord;
static float4 PS_IN_LocalColor;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float4 PS_IN_ScreenPosition : SCREEN_POSITION;
    float PS_IN_ZDepth : Z_DEPTH_VALUE;
    float4 PS_IN_ShadingPosition : SV_Position;
    float2 PS_IN_TexCoord : TEXCOORD0;
    float4 PS_IN_LocalColor : COLOR0;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float4 ParticleBase_Shading()
{
    return ParticleBase_ColorScale;
}

float4 o2S302C0_o0S2C0_ComputeColorFromStream_COLOR0_rgba__Compute(PS_STREAMS _streams)
{
    return clamp(_streams.LocalColor_id5, 0.0f.xxxx, 1.0f.xxxx);
}

float4 o2S302C0_o1S2C1_ComputeColorRadial_float4(PS_STREAMS _streams)
{
    float radialDistance = length(_streams.TexCoord_id4 - 0.5f.xx) * 2.0f;
    float4 unclamped = lerp(float4(1.5f, 0.0f, 1.5f, 1.0f), float4(1.5f, 1.5f, 0.0f, 0.0f), radialDistance.xxxx);
    float4 clamped = clamp(unclamped, 0.0f.xxxx, float4(1000.0f, 1000.0f, 1000.0f, 1.0f));
    float3 _163 = clamped.xyz * clamped.w;
    clamped = float4(_163.x, _163.y, _163.z, clamped.w);
    return clamped;
}

float4 o2S302C0_ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    float4 tex1 = o2S302C0_o0S2C0_ComputeColorFromStream_COLOR0_rgba__Compute(_streams);
    float4 tex2 = o2S302C0_o1S2C1_ComputeColorRadial_float4(_streams);
    float4 mix1 = tex1 * tex2;
    return mix1;
}

float4 ParticleComputeColorShader_Shading(PS_STREAMS _streams)
{
    float4 finalColor = ParticleBase_Shading() * o2S302C0_ComputeColorMultiply_Compute(_streams);
    return finalColor;
}

float ParticleUtilities_GetLinearDepth(float z)
{
    float fastA = -ParticleUtilities_ProjectionMatrix[2].z;
    float fastB = ParticleUtilities_ProjectionMatrix[3].z;
    return fastB / (z - fastA);
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xxxx, 0.0f, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx };
    _streams.ScreenPosition_id0 = PS_IN_ScreenPosition;
    _streams.ZDepth_id1 = PS_IN_ZDepth;
    _streams.ShadingPosition_id2 = PS_IN_ShadingPosition;
    _streams.TexCoord_id4 = PS_IN_TexCoord;
    _streams.LocalColor_id5 = PS_IN_LocalColor;
    float4 colorTarget = ParticleComputeColorShader_Shading(_streams);
    if (false)
    {
        float screenWidth = ParticleUtilities_ViewFrustum.x;
        float screenHeight = ParticleUtilities_ViewFrustum.y;
        float2 screenCoords = ((_streams.ScreenPosition_id0.xy / _streams.ScreenPosition_id0.ww) * 0.5f.xx) + 0.5f.xx;
        screenCoords.y = 1.0f - screenCoords.y;
        screenCoords = ParticleUtilities_Viewport.xy + (screenCoords * ParticleUtilities_Viewport.zw);
        float linearZOwn = -_streams.ZDepth_id1;
        float param = DepthBase_DepthStencil.Sample(Texturing_PointSampler, screenCoords).x;
        float linearZOpaque = ParticleUtilities_GetLinearDepth(param);
        float depthDistance = linearZOpaque - linearZOwn;
        float softEdge = smoothstep(0.0f, 1.0f, depthDistance * ParticleBase_SoftEdgeInverseDistance);
        colorTarget *= softEdge;
    }
    colorTarget.w *= ParticleBase_AlphaAdditive;
    _streams.ColorTarget_id3 = colorTarget;
    PS_OUT_ColorTarget = _streams.ColorTarget_id3;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_ScreenPosition = stage_input.PS_IN_ScreenPosition;
    PS_IN_ZDepth = stage_input.PS_IN_ZDepth;
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_LocalColor = stage_input.PS_IN_LocalColor;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
