cbuffer Globals
{
    int ShaderA_varA;
};

int frag_main()
{
    return ShaderA_varA;
}

void main()
{
    frag_main();
}
