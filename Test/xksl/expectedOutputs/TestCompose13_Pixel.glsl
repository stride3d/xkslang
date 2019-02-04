#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int Utils_Add(int i, int j)
{
    return i + j;
}

int o0S2C0_ShaderComputeColor_Compute(int i, int j)
{
    int param = i;
    int param_1 = j;
    return Utils_Add(param, param_1);
}

int main()
{
    int param = 0;
    int param_1 = 1;
    return o0S2C0_ShaderComputeColor_Compute(param, param_1);
}

