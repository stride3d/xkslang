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
} Base_globalCBuffer_var;

layout(std140) uniform shaderA_C1_0
{
    int Var1;
} shaderA_C1_0_var;

Base_streamBuffer Base_streamBuffer_var;
shaderA_streamBuffer shaderA_streamBuffer_var;

int main()
{
    return int(((((2.0 + Base_streamBuffer_var.Base_sbase1) + float(shaderA_C1_0_var.Var1)) + float(Base_globalCBuffer_var.Var1)) + 5.0) + float(shaderA_streamBuffer_var.shaderA_sa1));
}

