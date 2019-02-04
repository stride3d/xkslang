#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int Utils_compute()
{
    return 2;
}

int main()
{
    return Utils_compute();
}

