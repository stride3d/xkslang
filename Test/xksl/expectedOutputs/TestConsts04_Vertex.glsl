#version 450

struct VS_STREAMS
{
    float sbase1_id0;
};

layout(std140) uniform Globals
{
    int ShaderBase_Var0;
    int ShaderBase_Var1;
} Globals_var;

layout(location = 0) in float VS_IN_sbase1;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0);
    _streams.sbase1_id0 = VS_IN_sbase1;
    int i = int(((_streams.sbase1_id0 + float(Globals_var.ShaderBase_Var0)) + float(Globals_var.ShaderBase_Var1)) + 2.0);
}

