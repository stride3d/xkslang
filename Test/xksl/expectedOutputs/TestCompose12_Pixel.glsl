#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int o0S2C0_ShaderComputeColor_Compute(int i)
{
    return i;
}

int main()
{
    int param = 5;
    return o0S2C0_ShaderComputeColor_Compute(param);
}

