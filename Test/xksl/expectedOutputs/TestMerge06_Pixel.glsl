#version 450

struct globalStreams
{
    int sbase1_0;
};

layout(std140) uniform _globalCbuffer
{
    int Var1;
} _globalCbuffer_var;

globalStreams globalStreams_var;

int Base_ComputeBase()
{
    return _globalCbuffer_var.Var1 + globalStreams_var.sbase1_0;
}

int shaderA_f1()
{
    return Base_ComputeBase();
}

int main()
{
    return ((Base_ComputeBase() + _globalCbuffer_var.Var1) + Base_ComputeBase()) + shaderA_f1();
}

