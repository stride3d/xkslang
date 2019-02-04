#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int o5S2C0_o1S2C0_AnotherComp_Compute()
{
    return 5;
}

int o5S2C0_o2S2C1_AnotherComp_Compute()
{
    return 5;
}

int o0S12C0_DirectLightGroup_GetMaxLightCount()
{
    return 1;
}

int o6S12C0_DirectLightGroup_GetMaxLightCount()
{
    return 1;
}

int o5S2C0_o3S2C2_AnArrayComp_Compute()
{
    return 6;
}

int o5S2C0_o4S2C2_AnArrayComp_Compute()
{
    return 6;
}

void o5S2C0_MaterialSurfaceArray_Compute()
{
    int res = o5S2C0_o1S2C0_AnotherComp_Compute() + o5S2C0_o2S2C1_AnotherComp_Compute();
    res += o0S12C0_DirectLightGroup_GetMaxLightCount();
    res += o6S12C0_DirectLightGroup_GetMaxLightCount();
    res += o5S2C0_o3S2C2_AnArrayComp_Compute();
    res += o5S2C0_o4S2C2_AnArrayComp_Compute();
}

void main()
{
    o5S2C0_MaterialSurfaceArray_Compute();
}

