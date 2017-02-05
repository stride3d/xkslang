#version 450

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

int ShaderC_Compute()
{
    return ShaderB_Compute() + 4;
}

int main()
{
    return ShaderC_Compute();
}

