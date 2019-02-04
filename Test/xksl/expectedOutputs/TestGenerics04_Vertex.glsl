#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int ShaderBase_compute()
{
    return 1;
}

int main()
{
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
    return (ShaderBase_compute() + 3) + 4;
}

