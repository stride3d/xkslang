struct PS_STREAMS
{
    float2 TexCoord_id0;
};

cbuffer PerFrame
{
    float Global_Time;
    float Global_TimeStep;
};
cbuffer PerMaterial
{
    float o1S2C1_ComputeColorConstantFloatLink_constantFloat;
};

static float2 PS_IN_TexCoord;

struct SPIRV_Cross_Input
{
    float2 PS_IN_TexCoord : TEXCOORD0;
};

float4 o0S2C0_ComputeColorWave_5_0_01__0_03__Compute(PS_STREAMS _streams)
{
    float phase = length(_streams.TexCoord_id0 - 0.5f.xx);
    return (sin((((phase + (Global_Time * (-0.02999999932944774627685546875f))) * 2.0f) * 3.1400001049041748046875f) * 5.0f) * 0.00999999977648258209228515625f).xxxx;
}

float4 o1S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute()
{
    return float4(o1S2C1_ComputeColorConstantFloatLink_constantFloat, o1S2C1_ComputeColorConstantFloatLink_constantFloat, o1S2C1_ComputeColorConstantFloatLink_constantFloat, o1S2C1_ComputeColorConstantFloatLink_constantFloat);
}

float4 ComputeColorMultiply_Compute(PS_STREAMS _streams)
{
    float4 tex1 = o0S2C0_ComputeColorWave_5_0_01__0_03__Compute(_streams);
    float4 tex2 = o1S2C1_ComputeColorConstantFloatLink_Material_DisplacementValue__Compute();
    float4 mix1 = tex1 * tex2;
    return mix1;
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xx };
    _streams.TexCoord_id0 = PS_IN_TexCoord;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    frag_main();
}
