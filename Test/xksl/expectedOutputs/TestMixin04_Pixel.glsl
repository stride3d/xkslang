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

int main()
{
    return ShaderA_Compute();
}

