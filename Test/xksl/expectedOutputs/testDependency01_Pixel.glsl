#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int ShaderA_function()
{
    return 2;
}

int ShaderMain_function()
{
    return 1 + ShaderA_function();
}

int main()
{
    return ShaderMain_function();
}

