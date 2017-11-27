struct PS_STREAMS
{
    float2 TexCoord_id0;
    float4 ShadingPosition_id1;
    float4 ColorTarget_id2;
};

cbuffer Globals
{
    float RadiancePrefilteringGGXNoComputeShader_MipmapCount;
    float RadiancePrefilteringGGXNoComputeShader_Roughness;
    int RadiancePrefilteringGGXNoComputeShader_Face;
    int RadiancePrefilteringGGXNoComputeShader_RadianceMapSize;
};
TextureCube<float4> RadiancePrefilteringGGXNoComputeShader_RadianceMap;
SamplerState Texturing_LinearSampler;

static float2 PS_IN_TexCoord;
static float4 PS_IN_ShadingPosition;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float2 PS_IN_TexCoord : TEXCOORD0;
    float4 PS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float3 CubemapUtils_ConvertTexcoordsNoFlip(float2 inputTexcoord, int viewIndex)
{
    float2 position = (inputTexcoord * 2.0f) - float2(1.0f, 1.0f);
    if (viewIndex == 0)
    {
        return float3(1.0f, -position.y, -position.x);
    }
    if (viewIndex == 1)
    {
        return float3(-1.0f, -position.y, position.x);
    }
    if (viewIndex == 2)
    {
        return float3(position.x, 1.0f, position.y);
    }
    if (viewIndex == 3)
    {
        return float3(position.x, -1.0f, -position.y);
    }
    if (viewIndex == 4)
    {
        return float3(position.x, -position.y, 1.0f);
    }
    if (viewIndex == 5)
    {
        return float3(-position.x, -position.y, -1.0f);
    }
    return float3(0.0f, 0.0f, 0.0f);
}

float2 Hammersley_GetSamplePlane(int k, int samplesCount)
{
    float u = 0.0f;
    for (float p = 0.5f, kk = k; kk != int(0u); p *= 0.5f, kk = kk >> 1)
    {
        if ((kk & 1) != int(0u))
        {
            u += p;
        }
    }
    float v = (float(k) + 0.5f) / float(samplesCount);
    return float2(u, v);
}

float3 ImportanceSamplingGGX_GetSample(float2 xi, float roughness, float3 N)
{
    float a = roughness * roughness;
    float phi = 6.283185482025146484375f * xi.x;
    float CosTheta = sqrt((1.0f - xi.y) / (1.0f + (((a * a) - 1.0f) * xi.y)));
    float SinTheta = sqrt(1.0f - (CosTheta * CosTheta));
    float3 H;
    H.x = SinTheta * cos(phi);
    H.y = SinTheta * sin(phi);
    H.z = CosTheta;
    bool _190 = abs(N.z) < 0.999000012874603271484375f;
    bool3 _191 = bool3(_190, _190, _190);
    float3 UpVector = float3(_191.x ? float3(0.0f, 0.0f, 1.0f).x : float3(1.0f, 0.0f, 0.0f).x, _191.y ? float3(0.0f, 0.0f, 1.0f).y : float3(1.0f, 0.0f, 0.0f).y, _191.z ? float3(0.0f, 0.0f, 1.0f).z : float3(1.0f, 0.0f, 0.0f).z);
    float3 TangentX = normalize(cross(UpVector, N));
    float3 TangentY = cross(N, TangentX);
    return ((TangentX * H.x) + (TangentY * H.y)) + (N * H.z);
}

float BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0f)) + 1.0f, 9.9999997473787516355514526367188e-05f);
    return alphaR2 / ((3.1415927410125732421875f * d) * d);
}

float4 RadiancePrefilteringGGXNoComputeShader_4__Shading(PS_STREAMS _streams)
{
    float2 param = _streams.TexCoord_id0;
    int param_1 = RadiancePrefilteringGGXNoComputeShader_Face;
    float3 R = normalize(CubemapUtils_ConvertTexcoordsNoFlip(param, param_1));
    float4 prefilteredSample = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 prefilteredColor;
    float weight;
    for (int sampleIndex = 0; sampleIndex < 4; sampleIndex++)
    {
        int param_2 = sampleIndex;
        int param_3 = 4;
        float2 xi = Hammersley_GetSamplePlane(param_2, param_3);
        float2 param_4 = xi;
        float param_5 = RadiancePrefilteringGGXNoComputeShader_Roughness;
        float3 param_6 = R;
        float3 H = ImportanceSamplingGGX_GetSample(param_4, param_5, param_6);
        float3 L = (H * (2.0f * dot(R, H))) - R;
        float NoL = clamp(dot(R, L), 0.0f, 1.0f);
        float param_7 = RadiancePrefilteringGGXNoComputeShader_Roughness * RadiancePrefilteringGGXNoComputeShader_Roughness;
        float param_8 = NoL;
        float pdf = BRDFMicrofacet_NormalDistributionGGX(param_7, param_8) / 4.0f;
        float omegaS = 1.0f / (4.0f * pdf);
        float omegaP = 12.56637096405029296875f / ((6.0f * float(RadiancePrefilteringGGXNoComputeShader_RadianceMapSize)) * float(RadiancePrefilteringGGXNoComputeShader_RadianceMapSize));
        float mipLevel = clamp(0.5f * log2(omegaS / omegaP), 0.0f, RadiancePrefilteringGGXNoComputeShader_MipmapCount);
        prefilteredColor = float3(0.0f, 0.0f, 0.0f);
        weight = 0.0f;
        if (NoL > 0.0f)
        {
            weight = NoL;
            prefilteredColor = RadiancePrefilteringGGXNoComputeShader_RadianceMap.SampleLevel(Texturing_LinearSampler, L, mipLevel).xyz * weight;
        }
        prefilteredSample += float4(prefilteredColor, weight);
    }
    return prefilteredSample / float4(prefilteredSample.w, prefilteredSample.w, prefilteredSample.w, prefilteredSample.w);
}

void frag_main()
{
    PS_STREAMS _streams = { float2(0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id1 = PS_IN_ShadingPosition;
    _streams.ColorTarget_id2 = RadiancePrefilteringGGXNoComputeShader_4__Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}
