float2 ShaderTest_GetSamplePlane(int k, int samplesCount)
{
    float u = 0.0f;
    float p = 0.5f;
    int kk = k;
    for (; kk != int(0u); p *= 0.5f, kk = kk >> 1)
    {
        if ((kk & 1) != int(0u))
        {
            u += p;
        }
    }
    float v = (float(k) + 0.5f) / float(samplesCount);
    return float2(u, v);
}

void vert_main()
{
    int param = 5;
    int param_1 = 10;
    float2 toto = ShaderTest_GetSamplePlane(param, param_1);
}

void main()
{
    vert_main();
}
