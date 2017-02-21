#version 450

struct Base_streamBuffer
{
    int sbase1;
};

layout(std140) uniform Base_globalCBuffer
{
    int Var1;
} Base_globalCBuffer_var;

Base_streamBuffer Base_streamBuffer_var;

int shaderA_Compute1()
{
    return Base_globalCBuffer_var.Var1 + Base_streamBuffer_var.sbase1;
}

int shaderA_Compute2()
{
    return shaderA_Compute1();
}

int main()
{
    return shaderA_Compute1() + shaderA_Compute2();
}

