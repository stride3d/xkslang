#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

