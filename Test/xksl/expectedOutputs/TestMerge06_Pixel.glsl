#version 450

struct globalStreams
{
    int sbase1_0;
};

layout(std140) uniform Globals
{
    int Base_Var1;
} Globals_var;

globalStreams globalStreams_var;

int Base_ComputeBase()
{
    return Globals_var.Base_Var1 + globalStreams_var.sbase1_0;
}

int shaderA_f1()
{
    return Base_ComputeBase();
}

int main()
{
    return ((Base_ComputeBase() + Globals_var.Base_Var1) + Base_ComputeBase()) + shaderA_f1();
}

