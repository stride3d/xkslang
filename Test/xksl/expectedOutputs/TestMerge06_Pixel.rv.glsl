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

int Base_ComputeBase()
{
    return var_Base_globalCBuffer.Var1 + var_Base_streamBuffer.Base_sbase1;
}

int shaderA_f1()
{
    return Base_ComputeBase();
}

int main()
{
    return ((Base_ComputeBase() + var_Base_globalCBuffer.Var1) + Base_ComputeBase()) + shaderA_f1();
}

