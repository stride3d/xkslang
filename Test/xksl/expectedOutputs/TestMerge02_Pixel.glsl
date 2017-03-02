#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
    int sa1_2;
};

layout(std140) uniform shaderA_globalCBuffer
{
    int Var0;
} shaderA_globalCBuffer_var;

globalStreams globalStreams_var;

int main()
{
    return ((shaderA_globalCBuffer_var.Var0 + globalStreams_var.sa1_2) + 2) + 3;
}

