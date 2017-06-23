struct globalStreams
{
    int ColorTarget_0;
};

static globalStreams globalStreams_var;

int Base_Compute()
{
    return 1;
}

int A1_Compute()
{
    return Base_Compute() + 2;
}

int B1_Compute()
{
    return A1_Compute() + 3;
}

int A2_Compute()
{
    return B1_Compute() + 20;
}

int B2_Compute()
{
    return A2_Compute() + 30;
}

int C_Compute()
{
    return B2_Compute() + 100;
}

void frag_main()
{
    globalStreams_var.ColorTarget_0 = C_Compute();
}

void main()
{
    frag_main();
}
