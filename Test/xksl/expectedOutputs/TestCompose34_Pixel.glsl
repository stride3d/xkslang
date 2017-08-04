#version 450

struct PS_STREAMS
{
    float totoA_id0;
    float totoB_id1;
};

layout(std140) uniform Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
    float o2S39C0_ShaderComp_varC;
    float o4S39C1_ShaderComp_varC;
    float o4S39C1_o3S15C0_ShaderComp_varC;
} Globals_var;

layout(location = 0) in float PS_IN_totoA;
layout(location = 0) out float PS_OUT_totoB;

float ShaderMain_Compute()
{
    return 1.0;
}

float ShaderB_Compute()
{
    return ShaderMain_Compute() + 2.0;
}

float o2S39C0_ShaderComp_Compute()
{
    return Globals_var.o2S39C0_ShaderComp_varC;
}

float o4S39C1_ShaderComp_Compute()
{
    return Globals_var.o4S39C1_ShaderComp_varC;
}

float o4S39C1_o3S15C0_ShaderComp_Compute()
{
    return Globals_var.o4S39C1_o3S15C0_ShaderComp_varC;
}

float o4S39C1_ShaderCompBis_Compute()
{
    return o4S39C1_ShaderComp_Compute() + o4S39C1_o3S15C0_ShaderComp_Compute();
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0, 0.0);
    _streams.totoA_id0 = PS_IN_totoA;
    float f = ShaderB_Compute();
    _streams.totoB_id1 = ((f + o2S39C0_ShaderComp_Compute()) + o4S39C1_ShaderCompBis_Compute()) + _streams.totoA_id0;
    PS_OUT_totoB = _streams.totoB_id1;
}

