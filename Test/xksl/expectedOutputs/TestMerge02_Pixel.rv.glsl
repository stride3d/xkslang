#version 450

struct Base_streamBuffer
{
    float Base_sbase1;
    int Base_sbase2;
};

struct shaderA_streamBuffer
{
    int shaderA_sa1;
};

layout(std140) uniform shaderA_globalCBuffer
{
    int Var0;
} shaderA_globalCBuffer_var;

shaderA_streamBuffer shaderA_streamBuffer_var;

int main()
{
    return ((shaderA_globalCBuffer_var.Var0 + shaderA_streamBuffer_var.shaderA_sa1) + 2) + 3;
}

