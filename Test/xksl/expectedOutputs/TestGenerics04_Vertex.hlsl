int ShaderBase_compute()
{
    return 1;
}

int vert_main()
{
    return (ShaderBase_compute() + 3) + 4;
}

void main()
{
    vert_main();
}
