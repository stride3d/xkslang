#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int o0S12C0_TestDirectLightGroup1_GetMaxLightCount()
{
    return 1;
}

int o1S12C0_TestDirectLightGroup2_GetMaxLightCount()
{
    return 2;
}

int o3S12C0_TestDirectLightGroup3_GetMaxLightCount()
{
    return 3;
}

void o2S2C0_TestMaterialSurfaceArray_Compute()
{
    int res = 0;
    res += o0S12C0_TestDirectLightGroup1_GetMaxLightCount();
    res += o1S12C0_TestDirectLightGroup2_GetMaxLightCount();
    res += o3S12C0_TestDirectLightGroup3_GetMaxLightCount();
}

void main()
{
    o2S2C0_TestMaterialSurfaceArray_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

