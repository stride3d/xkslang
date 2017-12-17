#version 450

vec2 ShaderTest_GetSamplePlane(int k, int samplesCount)
{
    float u = 0.0;
    for (float p = 0.5, kk = k; kk != int(0u); p *= 0.5, kk = kk >> 1)
    {
        if ((kk & 1) != int(0u))
        {
            u += p;
        }
    }
    float v = (float(k) + 0.5) / float(samplesCount);
    return vec2(u, v);
}

void main()
{
    int param = 5;
    int param_1 = 10;
    vec2 toto = ShaderTest_GetSamplePlane(param, param_1);
}

