#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

int Base_Compute()
{
    return 1;
}

int main(float param1, float param2)
{
    return Base_Compute();
}

