#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
    int sa1_2;
};

layout(std140) uniform Base_globalCBuffer
{
    int Var0;
    int Var1;
} Base_globalCBuffer_var;

layout(std140) uniform shaderA_C1_0
{
    int Var1;
} shaderA_C1_0_var;

globalStreams globalStreams_var;

int main()
{
    return int(((((2.0 + globalStreams_var.sbase1_0) + float(shaderA_C1_0_var.Var1)) + float(Base_globalCBuffer_var.Var1)) + 5.0) + float(globalStreams_var.sa1_2));
}

