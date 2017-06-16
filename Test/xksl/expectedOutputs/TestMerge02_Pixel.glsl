#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
    int sa1_2;
};

layout(std140) uniform Globals
{
    int shaderA_Var0;
} Globals_var;

globalStreams globalStreams_var;

int main()
{
    return ((Globals_var.shaderA_Var0 + globalStreams_var.sa1_2) + 2) + 3;
}

