float3 ShaderMain_compute(float3 f)
{
    return f;
}

void vert_main()
{
    float3 param = float3(1.0f, 1.0f, 1.0f);
    float3 f3 = ShaderMain_compute(param);
}

void main()
{
    vert_main();
}
