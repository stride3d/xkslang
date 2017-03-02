#version 450

struct globalStreams
{
    int ColorTarget_0;
};

globalStreams globalStreams_var;

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
    globalStreams_var.ColorTarget_0 = ShaderB_Compute();
}

