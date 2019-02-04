#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int Utils_prout()
{
    return 5;
}

int UtilsBase_compute()
{
    return Utils_prout();
}

int Utils_compute()
{
    return UtilsBase_compute();
}

int main()
{
    return Utils_compute();
}

