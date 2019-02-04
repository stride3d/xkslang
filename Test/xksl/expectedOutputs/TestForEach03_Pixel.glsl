#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int main()
{
    int res = 0;
    res++;
    res += 2;
    res += 4;
    res += 4;
    res += 2;
    res += 4;
    res += 4;
    res++;
    res += 2;
    res += 4;
    res += 4;
    res += 2;
    res += 4;
    res += 4;
    return res;
}

