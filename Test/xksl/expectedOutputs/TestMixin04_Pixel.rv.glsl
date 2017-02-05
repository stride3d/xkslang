#version 450

int Base_Compute()
{
    return 1;
}

int ShaderA_Compute()
{
    return Base_Compute() + 2;
}

int main()
{
    return ShaderA_Compute();
}

