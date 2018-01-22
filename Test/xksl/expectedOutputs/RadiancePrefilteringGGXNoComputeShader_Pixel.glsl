#version 450

struct PS_STREAMS
{
    vec2 TexCoord_id0;
    vec4 ShadingPosition_id1;
    vec4 ColorTarget_id2;
};

layout(std140) uniform Globals
{
    float RadiancePrefilteringGGXNoComputeShader_MipmapCount;
    float RadiancePrefilteringGGXNoComputeShader_Roughness;
    int RadiancePrefilteringGGXNoComputeShader_Face;
    int RadiancePrefilteringGGXNoComputeShader_RadianceMapSize;
} Globals_var;

uniform samplerCube SPIRV_Cross_CombinedRadiancePrefilteringGGXNoComputeShader_RadianceMapTexturing_LinearSampler;

layout(location = 0) in vec2 PS_IN_TexCoord;
layout(location = 1) in vec4 PS_IN_ShadingPosition;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec3 CubemapUtils_ConvertTexcoordsNoFlip(vec2 inputTexcoord, int viewIndex)
{
    vec2 position = (inputTexcoord * 2.0) - vec2(1.0);
    if (viewIndex == 0)
    {
        return vec3(1.0, -position.y, -position.x);
    }
    if (viewIndex == 1)
    {
        return vec3(-1.0, -position.y, position.x);
    }
    if (viewIndex == 2)
    {
        return vec3(position.x, 1.0, position.y);
    }
    if (viewIndex == 3)
    {
        return vec3(position.x, -1.0, -position.y);
    }
    if (viewIndex == 4)
    {
        return vec3(position.x, -position.y, 1.0);
    }
    if (viewIndex == 5)
    {
        return vec3(-position.x, -position.y, -1.0);
    }
    return vec3(0.0);
}

vec2 Hammersley_GetSamplePlane(int k, int samplesCount)
{
    float u = 0.0;
    float p = 0.5;
    int kk = k;
    for (; kk != int(0u); p *= 0.5, kk = kk >> 1)
    {
        if ((kk & 1) != int(0u))
        {
            u += p;
        }
    }
    float v = (float(k) + 0.5) / float(samplesCount);
    return vec2(u, v);
}

vec3 ImportanceSamplingGGX_GetSample(vec2 xi, float roughness, vec3 N)
{
    float a = roughness * roughness;
    float phi = 6.283185482025146484375 * xi.x;
    float CosTheta = sqrt((1.0 - xi.y) / (1.0 + (((a * a) - 1.0) * xi.y)));
    float SinTheta = sqrt(1.0 - (CosTheta * CosTheta));
    vec3 H;
    H.x = SinTheta * cos(phi);
    H.y = SinTheta * sin(phi);
    H.z = CosTheta;
    vec3 UpVector = mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), bvec3(abs(N.z) < 0.999000012874603271484375));
    vec3 TangentX = normalize(cross(UpVector, N));
    vec3 TangentY = cross(N, TangentX);
    return ((TangentX * H.x) + (TangentY * H.y)) + (N * H.z);
}

float BRDFMicrofacet_NormalDistributionGGX(float alphaR, float nDotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(((nDotH * nDotH) * (alphaR2 - 1.0)) + 1.0, 9.9999997473787516355514526367188e-05);
    return alphaR2 / ((3.1415927410125732421875 * d) * d);
}

vec4 RadiancePrefilteringGGXNoComputeShader_1024__Shading(PS_STREAMS _streams)
{
    vec2 param = _streams.TexCoord_id0;
    int param_1 = Globals_var.RadiancePrefilteringGGXNoComputeShader_Face;
    vec3 R = normalize(CubemapUtils_ConvertTexcoordsNoFlip(param, param_1));
    vec4 prefilteredSample = vec4(0.0);
    vec3 prefilteredColor;
    float weight;
    for (int sampleIndex = 0; sampleIndex < 1024; sampleIndex++)
    {
        int param_2 = sampleIndex;
        int param_3 = 1024;
        vec2 xi = Hammersley_GetSamplePlane(param_2, param_3);
        vec2 param_4 = xi;
        float param_5 = Globals_var.RadiancePrefilteringGGXNoComputeShader_Roughness;
        vec3 param_6 = R;
        vec3 H = ImportanceSamplingGGX_GetSample(param_4, param_5, param_6);
        vec3 L = (H * (2.0 * dot(R, H))) - R;
        float NoL = clamp(dot(R, L), 0.0, 1.0);
        float param_7 = Globals_var.RadiancePrefilteringGGXNoComputeShader_Roughness * Globals_var.RadiancePrefilteringGGXNoComputeShader_Roughness;
        float param_8 = NoL;
        float pdf = BRDFMicrofacet_NormalDistributionGGX(param_7, param_8) / 4.0;
        float omegaS = 1.0 / (1024.0 * pdf);
        float omegaP = 12.56637096405029296875 / ((6.0 * float(Globals_var.RadiancePrefilteringGGXNoComputeShader_RadianceMapSize)) * float(Globals_var.RadiancePrefilteringGGXNoComputeShader_RadianceMapSize));
        float mipLevel = clamp(0.5 * log2(omegaS / omegaP), 0.0, Globals_var.RadiancePrefilteringGGXNoComputeShader_MipmapCount);
        prefilteredColor = vec3(0.0);
        weight = 0.0;
        if (NoL > 0.0)
        {
            weight = NoL;
            prefilteredColor = textureLod(SPIRV_Cross_CombinedRadiancePrefilteringGGXNoComputeShader_RadianceMapTexturing_LinearSampler, L, mipLevel).xyz * weight;
        }
        prefilteredSample += vec4(prefilteredColor, weight);
    }
    return prefilteredSample / vec4(prefilteredSample.w);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id1 = PS_IN_ShadingPosition;
    _streams.ColorTarget_id2 = RadiancePrefilteringGGXNoComputeShader_1024__Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}

