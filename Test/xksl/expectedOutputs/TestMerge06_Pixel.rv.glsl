#version 450

struct globalStreams
{
    int sbase1_0;
};

layout(std140) uniform Base_globalCBuffer
{
    int Var1;
} Base_globalCBuffer_var;

globalStreams globalStreams_var;

int Base_ComputeBase()
{
    return Base_globalCBuffer_var.Var1 + globalStreams_var.sbase1_0;
}

int shaderA_f1()
{
    return Base_ComputeBase();
}

int main()
{
    return ((Base_ComputeBase() + Base_globalCBuffer_var.Var1) + Base_ComputeBase()) + shaderA_f1();
}

