int ShaderA_function()
{
    return 2;
}

int ShaderMain_function()
{
    return 1 + ShaderA_function();
}

int frag_main()
{
    return ShaderMain_function();
}

void main()
{
    frag_main();
}
