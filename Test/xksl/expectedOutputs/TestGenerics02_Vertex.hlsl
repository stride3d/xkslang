cbuffer Globals
{
    float ShaderMain_ColorAberrations[5];
};

void vert_main()
{
    float res = 0.0f;
    for (int i = 0; i < 5; i++)
    {
        res += ShaderMain_ColorAberrations[i];
    }
}

void main()
{
    vert_main();
}
