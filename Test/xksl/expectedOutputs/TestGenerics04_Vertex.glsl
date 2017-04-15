#version 450

int ShaderBase_compute()
{
    return 1;
}

int main()
{
    return (ShaderBase_compute() + 3) + 4;
}

