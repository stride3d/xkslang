#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

void o1S2C1_TestMaterialSurfaceArray_Compute()
{
    int res = 0;
}

void main()
{
    o1S2C1_TestMaterialSurfaceArray_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

