#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int Utils_Negate(int i)
{
    return -i;
}

int o0S2C0_CompB_Compute(int i)
{
    int param = i;
    return Utils_Negate(param);
}

int main()
{
    int param = 1;
    return o0S2C0_CompB_Compute(param);
}

