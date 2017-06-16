#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
    int sa1_2;
};

layout(std140) uniform Globals
{
    int Base_Var1;
} Globals_var;

layout(std140) uniform C1
{
    int shaderA_Var1;
} C1_var;

globalStreams globalStreams_var;

int main()
{
    return int(((((2.0 + globalStreams_var.sbase1_0) + float(C1_var.shaderA_Var1)) + float(Globals_var.Base_Var1)) + 5.0) + float(globalStreams_var.sa1_2));
}

