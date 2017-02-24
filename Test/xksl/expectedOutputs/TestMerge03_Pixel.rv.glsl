#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
};

layout(std140) uniform Base_globalCBuffer
{
    int Var0;
    int Var1;
} Base_globalCBuffer_var;

layout(std140) uniform Base__0
{
    float Var2;
} Base__0_var;

globalStreams globalStreams_var;

int main()
{
    return int(((globalStreams_var.sbase1_0 + float(Base_globalCBuffer_var.Var1)) + Base__0_var.Var2) + 2.0);
}

