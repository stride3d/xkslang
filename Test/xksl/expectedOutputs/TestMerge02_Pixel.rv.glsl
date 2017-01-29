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
} var_shaderA_globalCBuffer;

shaderA_streamBuffer var_shaderA_streamBuffer;

int main()
{
    return ((var_shaderA_globalCBuffer.Var0 + var_shaderA_streamBuffer.shaderA_sa1) + 2) + 3;
}

