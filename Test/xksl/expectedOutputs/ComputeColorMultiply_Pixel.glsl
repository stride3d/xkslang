#version 450

struct PS_STREAMS
{
    vec2 TexCoord_id0;
};

layout(std140) uniform PerFrame
{
    float Global_Time;
    float Global_TimeStep;
} PerFrame_var;

layout(std140) uniform PerMaterial
{
    float o1S2C1_ComputeColorConstantFloatLink_constantFloat;
} PerMaterial_var;

layout(location = 0) in vec2 PS_IN_TexCoord;

vec4 o0S2C0_ComputeColorWave_5_0_01__0_03__Compute(PS_STREAMS _streams)
{
    float phase = length(_streams.TexCoord_id0 - vec2(0.5));
    return vec4(sin((((phase + (PerFrame_var.Global_Time * (-0.02999999932944774627685546875))) * 2.0) * 3.1400001049041748046875) * 5.0) * 0.00999999977648258209228515625);
}

vec4 o1S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute()
{
    return vec4(PerMaterial_var.o1S2C1_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o1S2C1_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o1S2C1_ComputeColorConstantFloatLink_constantFloat, PerMaterial_var.o1S2C1_ComputeColorConstantFloatLink_constantFloat);
}

vec4 ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    vec4 tex1 = o0S2C0_ComputeColorWave_5_0_01__0_03__Compute(_streams);
    vec4 tex2 = o1S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute();
    vec4 mix1 = tex1 * tex2;
    return mix1;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0));
    _streams.TexCoord_id0 = PS_IN_TexCoord;
}

