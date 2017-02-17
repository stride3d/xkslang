#version 450

int Math_Add(int i, int j)
{
    return i + j;
}

float ShaderA_compute()
{
    int param = 314;
    int param_1 = 2;
    return float(314 + Math_Add(param, param_1));
}

int Utils_DoSomething(float i, float j)
{
    return int(i + j);
}

float ShaderB_compute()
{
    float f = ShaderA_compute();
    float param = f;
    float param_1 = 1.0;
    f += float(Utils_DoSomething(param, param_1));
    return f;
}

float main()
{
    return ShaderB_compute();
}

