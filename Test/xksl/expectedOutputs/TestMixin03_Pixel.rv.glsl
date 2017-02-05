#version 450

struct Base_streamBuffer
{
    int Base_ColorTarget;
};

Base_streamBuffer Base_streamBuffer_var;

int Base_Compute()
{
    return 1;
}

int ShaderA_Compute()
{
    return Base_Compute() + 3;
}

void main()
{
    Base_streamBuffer_var.Base_ColorTarget = ShaderA_Compute();
}

