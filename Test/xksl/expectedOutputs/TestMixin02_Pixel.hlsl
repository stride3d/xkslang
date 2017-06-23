struct globalStreams
{
    int ColorTarget_s0;
};

static globalStreams globalStreams_var;

int OverrideB_Compute()
{
    return 5;
}

void frag_main()
{
    globalStreams_var.ColorTarget_s0 = 1 + OverrideB_Compute();
}

void main()
{
    frag_main();
}
