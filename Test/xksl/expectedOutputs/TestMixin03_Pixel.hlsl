struct globalStreams
{
    int ColorTarget_0;
};

static globalStreams globalStreams_var;

int Base_Compute()
{
    return 1;
}

int ShaderA_Compute()
{
    return Base_Compute() + 3;
}

void frag_main()
{
    globalStreams_var.ColorTarget_0 = ShaderA_Compute();
}

void main()
{
    frag_main();
}
