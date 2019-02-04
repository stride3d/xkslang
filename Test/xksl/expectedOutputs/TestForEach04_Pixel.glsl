#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int o1S2C1_ComputeColorA_Compute(int i)
{
    return -i;
}

int main()
{
    int res = 0;
    int param = res;
    res += o1S2C1_ComputeColorA_Compute(param);
    return res;
}

