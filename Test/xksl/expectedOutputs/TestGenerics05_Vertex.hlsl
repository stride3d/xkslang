cbuffer Globals
{
    float ShaderBase_2_5__aVar;
};

float ShaderBase_2_5__compute()
{
    return (ShaderBase_2_5__aVar + 5.0f) + 2.0f;
}

float vert_main()
{
    return ShaderBase_2_5__compute() + ShaderBase_2_5__aVar;
}

void main()
{
    vert_main();
}
