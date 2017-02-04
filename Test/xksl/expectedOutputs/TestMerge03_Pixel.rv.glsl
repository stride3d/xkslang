#version 450

struct Base_streamBuffer
{
    float Base_sbase1;
    int Base_sbase2;
};

layout(std140) uniform Base__0
{
    float Var2;
} Base__0_var;

layout(std140) uniform Base_globalCBuffer
{
    int Var0;
    int Var1;
} Base_globalCBuffer_var;

Base_streamBuffer Base_streamBuffer_var;

int main()
{
    return int(((Base_streamBuffer_var.Base_sbase1 + float(Base_globalCBuffer_var.Var1)) + Base__0_var.Var2) + 2.0);
}

