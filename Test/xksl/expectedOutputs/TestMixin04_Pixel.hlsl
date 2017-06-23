int Base_Compute()
{
    return 1;
}

int ShaderA_Compute()
{
    return Base_Compute() + 2;
}

int frag_main()
{
    return ShaderA_Compute();
}

void main()
{
    frag_main();
}
