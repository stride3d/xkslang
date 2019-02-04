cbuffer PerView
{
    float4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_padding_PerView_Default;
    float3 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[9];
    float4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_padding_PerView_Lighting;
};

void o1S2C0_EnvironmentLight_PrepareEnvironmentLight()
{
}

float4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_EvaluateSphericalHarmonics(float3 sphericalColors[9], float3 direction)
{
    return float4(sphericalColors[0], 1.0f);
}

float4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_Compute(float3 direction)
{
    float3 param[9];
    param[0] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[0];
    param[1] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[1];
    param[2] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[2];
    param[3] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[3];
    param[4] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[4];
    param[5] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[5];
    param[6] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[6];
    param[7] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[7];
    param[8] = o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[8];
    float3 param_1 = direction;
    return o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_EvaluateSphericalHarmonics(param, param_1);
}

void o1S2C0_LightSkyboxShader_PrepareEnvironmentLight()
{
    o1S2C0_EnvironmentLight_PrepareEnvironmentLight();
    float3 param = 0.0f.xxx;
}

void vert_main()
{
    o1S2C0_LightSkyboxShader_PrepareEnvironmentLight();
}

void main()
{
    vert_main();
}
