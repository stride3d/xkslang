#version 450

struct PS_STREAMS
{
    int sbase1_id0;
};

layout(std140) uniform Globals
{
    int Base_Var1;
} Globals_var;

layout(location = 0) in int PS_IN_sbase1;

int shaderA_Compute1(PS_STREAMS _streams)
{
    return Globals_var.Base_Var1 + _streams.sbase1_id0;
}

int shaderA_Compute2(PS_STREAMS _streams)
{
    return shaderA_Compute1(_streams);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.sbase1_id0 = PS_IN_sbase1;
    int i = shaderA_Compute1(_streams) + shaderA_Compute2(_streams);
}

