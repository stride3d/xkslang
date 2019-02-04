#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    int ColorTarget_id0;
};

out int PS_OUT_ColorTarget;

int Base_Compute()
{
    return 1;
}

int ShaderA_Compute()
{
    return Base_Compute() + 3;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.ColorTarget_id0 = ShaderA_Compute();
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

