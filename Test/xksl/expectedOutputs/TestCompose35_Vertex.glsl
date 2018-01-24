#version 450

layout(std140) uniform PerView
{
    vec4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor__padding_PerView_Default;
    vec3 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[9];
    vec4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor__padding_PerView_Lighting;
} PerView_var;

void o1S2C0_EnvironmentLight_PrepareEnvironmentLight()
{
}

vec4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_EvaluateSphericalHarmonics(vec3 sphericalColors[9], vec3 direction)
{
    return vec4(sphericalColors[0], 1.0);
}

vec4 o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_Compute(vec3 direction)
{
    vec3 param[9];
    param[0] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[0];
    param[1] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[1];
    param[2] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[2];
    param[3] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[3];
    param[4] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[4];
    param[5] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[5];
    param[6] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[6];
    param[7] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[7];
    param[8] = PerView_var.o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_SphericalColors[8];
    vec3 param_1 = direction;
    return o1S2C0_o0S2C0_SphericalHarmonicsEnvironmentColor_EvaluateSphericalHarmonics(param, param_1);
}

void o1S2C0_LightSkyboxShader_PrepareEnvironmentLight()
{
    o1S2C0_EnvironmentLight_PrepareEnvironmentLight();
    vec3 param = vec3(0.0);
}

void main()
{
    o1S2C0_LightSkyboxShader_PrepareEnvironmentLight();
}

