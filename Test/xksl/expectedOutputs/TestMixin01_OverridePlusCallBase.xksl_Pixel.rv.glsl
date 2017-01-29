#version 450

struct Base_streamBuffer
{
    int Base_ColorTarget;
};

Base_streamBuffer var_Base_streamBuffer;

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
    var_Base_streamBuffer.Base_ColorTarget = ShaderA_Compute();
}

