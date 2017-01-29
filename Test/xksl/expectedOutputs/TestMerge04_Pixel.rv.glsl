#version 450

struct Base_streamBuffer
{
    float Base_sbase1;
    int Base_sbase2;
};

struct shaderA_streamBuffer
{
    int shaderA_sa1;
};

layout(std140) uniform Base_globalCBuffer
{
    int Var0;
    int Var1;
} var_Base_globalCBuffer;

layout(std140) uniform shaderA_C1_0
{
    int Var1;
} var_shaderA_C1_0;

Base_streamBuffer var_Base_streamBuffer;
shaderA_streamBuffer var_shaderA_streamBuffer;

int main()
{
    return int(((((2.0 + var_Base_streamBuffer.Base_sbase1) + float(var_shaderA_C1_0.Var1)) + float(var_Base_globalCBuffer.Var1)) + 5.0) + float(var_shaderA_streamBuffer.shaderA_sa1));
}

