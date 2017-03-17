#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
    int sa1_2;
};

layout(std140) uniform globalCbuffer
{
    int shaderA_Var0;
} globalCbuffer_var;

globalStreams globalStreams_var;

int main()
{
    return ((globalCbuffer_var.shaderA_Var0 + globalStreams_var.sa1_2) + 2) + 3;
}

