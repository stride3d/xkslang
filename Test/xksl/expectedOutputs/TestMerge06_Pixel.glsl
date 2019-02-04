#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    int sbase1_id0;
};

layout(std140) uniform Globals
{
    int Base_Var1;
} Globals_var;

in int PS_IN_SBASE1;

int Base_ComputeBase(PS_STREAMS _streams)
{
    return Globals_var.Base_Var1 + _streams.sbase1_id0;
}

int shaderA_f1(PS_STREAMS _streams)
{
    return Base_ComputeBase(_streams);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.sbase1_id0 = PS_IN_SBASE1;
    int i = ((Base_ComputeBase(_streams) + Globals_var.Base_Var1) + Base_ComputeBase(_streams)) + shaderA_f1(_streams);
}

