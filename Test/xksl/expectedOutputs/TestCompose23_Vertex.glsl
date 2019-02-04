#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int o0S12C0_TestDirectLightGroup_Compute()
{
    return 1;
}

int o1S12C0_TestDirectLightGroup_Compute()
{
    return 1;
}

void main()
{
    int res;
    res += o0S12C0_TestDirectLightGroup_Compute();
    res += o1S12C0_TestDirectLightGroup_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

