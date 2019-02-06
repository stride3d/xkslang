struct PS_STREAMS
{
    float2 TexCoord_id0;
};

static const PS_STREAMS _45 = { 0.0f.xx };

cbuffer PerFrame
{
    float Global_Time;
    float Global_TimeStep;
};

static float2 PS_IN_TEXCOORD0;

struct SPIRV_Cross_Input
{
    float2 PS_IN_TEXCOORD0 : TEXCOORD0;
};

float4 ComputeColorWave_1_2_3_645541__Compute(PS_STREAMS _streams)
{
    float phase = length(_streams.TexCoord_id0 - 0.5f.xx);
    return (sin((((phase + (Global_Time * 3.6455414295196533203125f)) * 2.0f) * 3.1400001049041748046875f) * 1.0f) * 2.0f).xxxx;
}

void frag_main()
{
    PS_STREAMS _streams = _45;
    _streams.TexCoord_id0 = PS_IN_TEXCOORD0;
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TEXCOORD0 = stage_input.PS_IN_TEXCOORD0;
    frag_main();
}
