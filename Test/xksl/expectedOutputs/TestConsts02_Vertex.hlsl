cbuffer Globals
{
    float ShaderMain_anArray[25];
};

void vert_main()
{
    float toto = 0.0f;
    for (int i = 0; i < 25; i++)
    {
        toto += ShaderMain_anArray[i];
    }
}

void main()
{
    vert_main();
}
