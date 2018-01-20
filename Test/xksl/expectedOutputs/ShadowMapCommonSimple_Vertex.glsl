#version 450

uniform sampler2DShadow SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureShadowMapCommon_LinearClampCompareLessEqualSampler;
uniform sampler2D SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureShadowMapCommon_LinearBorderSampler;

float ShadowMapCommon_SampleTextureAndCompare(vec2 position, float positionDepth)
{
    vec3 _13 = vec3(position, positionDepth);
    return textureLod(SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureShadowMapCommon_LinearClampCompareLessEqualSampler, vec3(_13.xy, _13.z), 0.0);
}

float ShadowMapCommon_SampleThickness()
{
    float shadowMapDepth = textureLod(SPIRV_Cross_CombinedShadowMapCommon_ShadowMapTextureShadowMapCommon_LinearBorderSampler, vec2(0.5), 0.0).x;
    return shadowMapDepth;
}

void main()
{
    vec2 param = vec2(0.5);
    float param_1 = 1.0;
    float f1 = ShadowMapCommon_SampleTextureAndCompare(param, param_1);
    float f2 = ShadowMapCommon_SampleThickness();
}

