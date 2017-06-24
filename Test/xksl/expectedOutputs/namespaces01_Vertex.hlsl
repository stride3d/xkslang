int ShaderA_compute()
{
    return 3;
}

int vert_main()
{
    return ShaderA_compute();
}

void main()
{
    vert_main();
}
