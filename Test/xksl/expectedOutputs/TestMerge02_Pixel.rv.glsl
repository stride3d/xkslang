#version 450

struct Base_streamBuffer
{
    float sbase1;
    int sbase2;
};

struct shaderA_streamBuffer
{
    int sa1;
};

layout(std140) uniform shaderA_globalCBuffer
{
    int Var0;
} shaderA_globalCBuffer_var;

shaderA_streamBuffer shaderA_streamBuffer_var;

int main()
{
    return ((shaderA_globalCBuffer_var.Var0 + shaderA_streamBuffer_var.sa1) + 2) + 3;
}

