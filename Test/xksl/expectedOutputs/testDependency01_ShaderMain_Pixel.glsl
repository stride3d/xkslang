#version 450

int ShaderA_function()
{
    return 2;
}

int ShaderMain_function()
{
    return 1 + ShaderA_function();
}

int main()
{
    return ShaderMain_function();
}

