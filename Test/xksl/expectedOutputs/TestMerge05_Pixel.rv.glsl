#version 450

struct Base_streamBuffer
{
    int Base_sbase1;
};

layout(std140) uniform Base_globalCBuffer
{
    int Var1;
} var_Base_globalCBuffer;

Base_streamBuffer var_Base_streamBuffer;

int shaderA_Compute1()
{
    return var_Base_globalCBuffer.Var1 + var_Base_streamBuffer.Base_sbase1;
}

int shaderA_Compute2()
{
    return shaderA_Compute1();
}

int main()
{
    return shaderA_Compute1() + shaderA_Compute2();
}

