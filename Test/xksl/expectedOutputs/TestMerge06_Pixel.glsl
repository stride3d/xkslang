#version 450

struct globalStreams
{
    int sbase1_0;
};

layout(std140) uniform globalCbuffer
{
    int Var1;
} globalCbuffer_var;

globalStreams globalStreams_var;

int Base_ComputeBase()
{
    return globalCbuffer_var.Var1 + globalStreams_var.sbase1_0;
}

int shaderA_f1()
{
    return Base_ComputeBase();
}

int main()
{
    return ((Base_ComputeBase() + globalCbuffer_var.Var1) + Base_ComputeBase()) + shaderA_f1();
}

