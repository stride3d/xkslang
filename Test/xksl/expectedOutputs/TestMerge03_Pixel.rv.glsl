#version 450

struct Base_streamBuffer
{
    float Base_sbase1;
    int Base_sbase2;
};

layout(std140) uniform Base__0
{
    float Var2;
} var_Base__0;

layout(std140) uniform Base_globalCBuffer
{
    int Var0;
    int Var1;
} var_Base_globalCBuffer;

Base_streamBuffer var_Base_streamBuffer;

int main()
{
    return int(((var_Base_streamBuffer.Base_sbase1 + float(var_Base_globalCBuffer.Var1)) + var_Base__0.Var2) + 2.0);
}

