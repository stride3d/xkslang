Texture2D<float4> ShadowMapCommon_ShadowMapTexture;
SamplerState ShadowMapCommon_LinearBorderSampler;
SamplerComparisonState ShadowMapCommon_LinearClampCompareLessEqualSampler;

float ShadowMapCommon_SampleTextureAndCompare(float2 position, float positionDepth)
{
    float3 _13 = float3(position, positionDepth);
    return ShadowMapCommon_ShadowMapTexture.SampleCmpLevelZero(ShadowMapCommon_LinearClampCompareLessEqualSampler, _13.xy, _13.z);
}

float ShadowMapCommon_SampleThickness()
{
    float shadowMapDepth = ShadowMapCommon_ShadowMapTexture.SampleLevel(ShadowMapCommon_LinearBorderSampler, float2(0.5f, 0.5f), 0.0f).x;
    return shadowMapDepth;
}

void vert_main()
{
    float2 param = float2(0.5f, 0.5f);
    float param_1 = 1.0f;
    float f1 = ShadowMapCommon_SampleTextureAndCompare(param, param_1);
    float f2 = ShadowMapCommon_SampleThickness();
}

void main()
{
    vert_main();
}
