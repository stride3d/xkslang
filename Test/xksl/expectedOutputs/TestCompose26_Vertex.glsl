#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int o0S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return 1;
}

int o2S2C0_o1S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return 1;
}

void o2S2C0_TestMaterialSurfaceArray_Compute()
{
    int res = 0;
    res += o0S12C0_TestDirectLightGroup_GetMaxLightCount();
    res += o2S2C0_o1S12C0_TestDirectLightGroup_GetMaxLightCount();
}

void main()
{
    o2S2C0_TestMaterialSurfaceArray_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

