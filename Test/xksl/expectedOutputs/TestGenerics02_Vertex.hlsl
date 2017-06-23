cbuffer Globals
{
    float ShaderMain_5__ColorAberrations[5];
};

void vert_main()
{
    float res = 0.0f;
    for (int i = 0; i < 5; i++)
    {
        res += ShaderMain_5__ColorAberrations[i];
    }
}

void main()
{
    vert_main();
}
