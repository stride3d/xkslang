#version 450

struct Base_streamBuffer
{
    int ColorTarget;
};

Base_streamBuffer Base_streamBuffer_var;

int Base_Compute()
{
    return 1;
}

int ShaderA_Compute()
{
    return Base_Compute() + 2;
}

int ShaderB_Compute()
{
    return ShaderA_Compute() + 3;
}

void main()
{
    Base_streamBuffer_var.ColorTarget = ShaderB_Compute();
}

